#pragma once

typedef struct _FILE_SIGNATURE {
    const WCHAR* Extension; 
    const UCHAR* Signature;
    size_t SigLength;
} FILE_SIGNATURE;


BOOLEAN CmpFileSignatures(PUNICODE_STRING OriExt, ULONG Length, LONGLONG ByteOffset, PVOID Buffer);