rule KKB-2508-001_PE
{
    meta:
        description = "KKB-2508-001 PE FILE Deny"
        author = "노무승"
        date = "2025-08-25"

    strings:
        $go_build_id1 = "Go build ID: \"3JAJzjp8c1axum8N-ux7/5xjfhaDqmFLQCjgGaV0y/KJL9EBymZ4frDk-t2sAf/nhX1hw5MjrJN7LU9jxNu\""
        $go_build_id2 = "Go build ID: \"wMrg8E8z6_5r3RNtTrFi/skEqFWaSCrbQmMbY7c9Z/gNkHqQVUBd_d9Si5uIjc/hkWIGAGZg0m0jEVa8Rfd\""
        $c2c_str      = "drmon.chickenkiller.com"

    condition:
        uint16(0) == 0x5A4D and
        ($go_build_id1 or $go_build_id2) and
        $c2c_str
}
