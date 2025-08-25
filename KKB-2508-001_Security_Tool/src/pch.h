#pragma once
#include <fltKernel.h>
#include <dontuse.h>
#include <ntddk.h>
#include <ntstrsafe.h>

typedef PCHAR(*GET_PROCESS_IMAGE_NAME) (PEPROCESS Process);
GET_PROCESS_IMAGE_NAME PsGetProcessImageFileName;

#include "setting.h"
#include "global.h"
#include "util.h"

#include "decoyInit.h"
#include "signature.h"
#include "operation.h"
#include "registration.h"