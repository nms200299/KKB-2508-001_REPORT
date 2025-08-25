#include "pch.h"
#include "signature.h"

FILE_SIGNATURE FileSignatures[] = {
	{L"avi", (UCHAR*)"\x52\x49\x46\x46", 4},
	{L"doc", (UCHAR*)"\xD0\xCF\x11\xE0", 4},
	{L"docx", (UCHAR*)"\x50\x4B\x03\x04", 4},
	{L"hwp", (UCHAR*)"\xD0\xCF\x11\xE0", 4},
	{L"jpg", (UCHAR*)"\xFF\xD8\xFF", 3},
	{L"jpeg", (UCHAR*)"\xFF\xD8\xFF", 3},
	{L"mp4", (UCHAR*)"\x00\x00\x00\x14\x66\x74\x79\x70\x69\x73\x6F\x6D", 12 },
	{L"pdf", (UCHAR*)"\x25\x50\x44\x46", 4},
	{L"png", (UCHAR*)"\x89\x50\x4E\x47", 4},
	{L"ppt", (UCHAR*)"\xD0\xCF\x11\xE0", 4},
	{L"pptx", (UCHAR*)"\x50\x4B\x03\x04", 4},
	{L"xls", (UCHAR*)"\xD0\xCF\x11\xE0", 4},
	{L"xlsx", (UCHAR*)"\x50\x4B\x03\x04", 4},
	{L"zip", (UCHAR*)"\x50\x4B\x03\x04", 4},
};

#define FILE_SIGNATURE_COUNT (sizeof(FileSignatures)/sizeof(FileSignatures[0]))


BOOLEAN CmpFileSignatures(PUNICODE_STRING OriExt, ULONG Length, LONGLONG ByteOffset, PVOID Buffer){
	if (OriExt == NULL || OriExt->Buffer == NULL) {
		return FALSE;
	}
	if (OriExt->Length > OriExt->MaximumLength) {
		return FALSE;
	}
	if (Buffer == NULL) {
		return FALSE;
	}

	for (int idx = 0; idx < FILE_SIGNATURE_COUNT; idx++) {
		UNICODE_STRING CmpExt;
		RtlInitUnicodeString(&CmpExt, FileSignatures[idx].Extension);

		if (RtlCompareUnicodeString(OriExt, &CmpExt, TRUE) != 0) {
			continue;
		} // 확장자 불일치

		if (ByteOffset >= (LONGLONG)FileSignatures[idx].SigLength) {
			return FALSE;
		} // 수정하는 오프셋이 파일 시그니처를 벗어남

		for (int loop = 0; ((ULONG)loop < Length) && ((ByteOffset+(LONGLONG)loop) < (LONGLONG)FileSignatures[idx].SigLength); loop++) {
			// 탐색할 부분이 쓸 전체 길이를 넘지 않아야 하고, 시그니처 길이를 넘지 않아야 함.
			if (FileSignatures[idx].Signature[ByteOffset+loop] != ((PUCHAR)Buffer)[loop]) {
				return TRUE;
			}
		}
	}
	return FALSE;
}
