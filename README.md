# -Unreal
MTVS Final Project

**일정 : 2024.10.01 ~ 2024. 11.29**
---
**마일스톤으로 큰단위의 작업을 설정해 주세요**

**마일스톤 컨벤션**
- 프로토타입, 알파, 베타 순으로 개인으로 마일스톤 작성

**마일스톤 컨벤션 예시**
  
- Title : CSR_prototype

<br/> 

**이슈 작성 후 작업 진행 바랍니다.**

<br/> 

**이슈 예**

**[Feat] map 요청 api 작성**

### ISSUE

- Type: feat
- Detail: c++ 플레이어가 선택한 map에 대한 umap과 datatable을 요청한다.

### TODO

- [ ]  http req 셋팅
- [ ]  요청할 맵 식별 id 전달
- [ ]  rep 함수 선언

<br/> 

**작은 변경사항이라도 꼭 중간마다 커밋, 푸쉬 부탁드립니다.**

<br/> 

**push 전에는 서로 공유 부탁드립니다.**
<br/> 

**브렌치명 컨벤션**
- 브렌치 명 : <자기이니셜>_<기능>

**브렌치명 컨벤션 예**

- CSR_PlayerJump
- YWJ-ClickButtonAndExecSomething

<br/> 

**커밋 컨벤션**

- <유형> : <내용> #이슈번호
- Feat : 새로운 기능을 추가할 경우
- Fix : 버그를 고친 경우
- !HOTFIX : 치명적인 버그를 고친 경우
- Style : Norm 규정, 세미콜론 누락, 코드수정이 없는 경우
- Refactor : 리팩토링
- Comment : 주석 추가 및 변경
- Docs : 문서를 수정한 경우
- Test : 테스트 추가
- Rename : 파일 혹은 폴더명을 수정하거나 옮기는 작업만인 경우
- Remove : 파일을 삭제하는 경우

**커밋 컨벤션 예**

- Feat: tokenizer 구현 - minseok2
- Fix: tokenizer 문자열 파싱 버그 수정 - junlee2
- Remove: test.txt 임시파일 삭제 - tyi
- Rename: test.txt → operator.txt - jincpark

<br/> 

**코드 컨벤션**

### 모든 c++ 클래스는 public으로 설정하고 자신 이름 폴더로 저장합니다.
- 변수 - 카멜 케이스(Camel Case) : int playerNumber
- 함수, 클래스 - 파스칼 케이스((Pascal Case) : void MoveSpeedHigh();
- 상수나 Define 은 대문자로 작성 : const String URL = "127.0.0.1";
- c++ 클래스 생성 시 {자기이니셜}_ : CSR_PlayerActor
- RPC 함수 관련
Client에서 Server로 요청하는 경우 SRPC_{함수 이름}
Server에서 해당 Client로 요청하는 경우 CRPC_{함수 이름}
Server에서 모든 Client로 요청하는 경우 MRPC_{함수 이름}
- API요청 시
Request는 REQ_{함수 이름}
Response는 RES_{함수 이름}
- 블루프린트 생성 접두어 BP_
- 애니메이션 블루프린트 생성 시 접두어 ABP_
- 위젯 블루프린트 생성 시 접두어 WBP_


