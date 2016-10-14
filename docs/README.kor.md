![](logo.png)


# **데스크탑 런처**
* 크로스플랫폼
* 높은 확장성 과 사용자 맞춤형 설정 지원
* 플러그인 기반 아키텍처
* 온라인 동기화 지원
* FOSS 기반 개발

# 빌드 방법
## Windows
### 준비물
* Qt 5.7 이상
* Visual Studio 2015 이상 

### 빌드
* `qmake`와 `nmake`가 호출이 가능하도록 PATH를 설정합니다.
  * `qmake`는 QT에서 제공되는 빌드 툴입니다. 예를 들면 이런 경로에서 찾을 수 있습니다.
    >  `C:\QT\Qt5.7.0\5.7\msvc2015\bin\qmake.exe`
* 이 스크립트를 실행합니다. `script/build-windows.bat release`. 디버그 모드로 빌드하려면 `debug` 대신에 `release`를 지시합니다.
* 성공하면, 리파지토리 루트 아래 `output` 디렉토리에 산출물이 복사됩니다. 

## 리눅스
### 준비물
* Qt 5.7 이상

### 빌드
* `qmake` 명령어가 호출이 가능하도록 PATH를 설정합니다. 또한 C++11 문법이 컴파일 가능한 GCC 버전이 설치되어 있어야 합니다.
* `script/build-linux.sh`을 실행합니다.

# 스크린샷
![](screenshot-google-search.png)

![](screenshot-file.png)

# 라이센스
[Mozilla Public License Version 2.0](https://www.mozilla.org/en-US/MPL/2.0/) 