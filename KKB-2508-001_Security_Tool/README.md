# KKB-2508-001 Security_Tool

* KKB-2508-001 랜섬웨어 보안 도구입니다.
  * 실시간 파일 시스템 모니터링
  * 미끼 파일을 이용한 랜섬웨어 차단 기능
  * 파일 시그니쳐 변조 탐지를 통한 랜섬웨어 차단 기능
  * 배치 파일을 통한 기능 옵션화
  * 간편 설치 및 삭제 도구 지원

* KKB-2508-001 랜섬웨어의 PE 아키텍처가 x64라, x64 OS에 호환되게 제작되었습니다.

---

## 개발 환경

 * Visual Studio 2019

 * Windows 10 x64 22H2

## 테스트 환경

 * Windows 10 x64 22H2

## 사용법

1. 1_TestModeActive.bat를 실행하여 테스트 모드를 활성화해주세요.
2. 2_Installer.bat를 실행하여 설치한 후, 옵션에 맞는 기능을 선택해주시면 됩니다.
3. C:\Program Files\RswDetect 폴더에 들어가 RswDetect.inf를 우클릭하여 설치를 눌러주세요.
4. cmd를 관리자 권한으로 실행해 주시고 fltmc load RswDetect를 입력해 서비스를 실행시켜주세요.
5. 삭제 시에는 3_Uninstaller.bat를 실행하시면 됩니다.

### 테스트 사진


