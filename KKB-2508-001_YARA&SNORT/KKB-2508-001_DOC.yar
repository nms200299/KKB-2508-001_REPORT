rule KKB-2508-001_DOC
{
    meta:
        description = "KKB-2508-001 DOC FILE Deny"
        author = "노무승"
        date = "2025-08-25"

    strings:
        $sig1 = "TVqQAAMAAAAEAAAA//8AAIsAAAAAAAAA"
        $sig2 = "pAClEKUgpTClQKVQpWClcKWApZCloKWw"
        $sig3 = "MKdIp2CncKeIp6CnsKfgp/CnCKggqDCo"

    condition:
        uint16(0) == 0xCFD0 and
        $s1 and $s2 and $s3
}