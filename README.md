# vnc_connector
ssh 접속 또는 vncserver 를 가동해야 하는 경우에  
접속 해야할 컴퓨터 및 user가 많이 있다면 리스트에서 선택할 수 있으며  
특별한 기능은 없지만...  
단지 터미널 창을 띄우고 복사를 미리 해주는 기능이 있다.  
> ssh 터널링 같은 경우 긴 명령어 및 옵션를 쉽게 사용 가능

사용 kit 버전 : Qt 6.4.0   
IDE: qtcreator (필요)  
의존성 프로그램: terminator (필요)

> 현재 빌드된 파일로만 실행은 불가능 하며, qt 6. 이상을 빌드하기 위해서 qtcreator 등이 필요   
TODO: qt 버전을 낮춰서 사용할 수 있는지 추후 찾아보기(보통 qt5 는 설치되어 있는 듯 하다.)   

## 빌드
깃 클론   
```
mkdir qt_ws
cd qt_ws
git clone https://github.com/terrificmn/vnc_connector.git
```

qtcreator 를 이용 OpenProject 를 해준다.   
깃 클론 한 디렉토리를 열어준다. 예) ~/qt_ws/vnc_connector  
이후 현재 프로젝트는 CMake 빌드 이므로 Open File 시에 CMakeLists.txt 를 지정해서 열어준다.

최초 configure가 필요할 경우 kit 등을 맞춰준 후에 빌드해주면 된다.

Release 버전으로 빌드 하면 바로 상위 경로에 디렉토리가 만들어진다. build-vnc_connector-Desktop-Release/   
여기에 *appvnc_connector* 실행 파일이 생기는데 이를 원하는 위치로 복사 시켜 사용한다.  

먼저 디렉토리를 (예 vnc_connector, 편의상 패키지 이름과 같게..)만들어서 이동시켜준다. (깃 클론 디렉토리와 다른 디렉토리)   
깃에 있는 list 디렉토리도 같이 복사해준다 (list/example.yaml 파일도 복사)   

예
```
mkdir ~/vnc_connector
mv ~/qt_ws/build-vnc_connector-kit버전-Release/appvnc_connector ~/vnc_connector/
cp -r ~/qt_ws/vnc_connector/list ~/vnc_connector/
```

> build kit이름에 따라 달라지나, 보통 Desktop-Release 처럼 사용됨  

> 최초 qt_ws 를 workspace를 만들지 않았다면 vnc_connetor 패키지 디렉토리 안에 build 디렉토리가 생기고  
그 안에 Desktop-Release, Desktop-Debug 등으로 빌드가 된다.   
그러므로 특정 디렉토리를 project 저장 장소를 하려면 새로운 프로젝트를 만들고 (아무거나)   
이후 Proejct Location 정할 때 경로 및 디렉토리를 정하고 Use as default project location 에 체크를 해주면 된다.  
그렇게 대충대충 기본값으로 next, next를 눌러서 finish 시켜준다. (이후 뉴프로젝트는 지워준다)  


복사된 yaml파일의 데이터를 수정한다    
이름은 반드시 **list.yaml 로 바꾼다**  
- com_name1,2,3,4 는 이름 변경해도 됨 (별칭 처럼 사용)
- 2번째 카테고리인 각 id, ip, port, ssh 는 key는 변경 금지, value(값)들만 변경해서 사용

터미널에 쉽게 실행 가능하게 심볼릭 링크 만들기 
```
sudo ln -s ~/vnc_connector/appvnc_connector /usr/bin/vnc_connector
```
현재 버전에서는 **/usr/bin/vnc_connector 만 지원**하기 때문에 (고정) 다른 곳으로 심링크를 걸어주면 config 파일을 못찾게 됨   

단, 빌드파일의 위치는 어디는 상관 없음

추후 버전에서 수정 예정


## yaml 파일의 리스트 업데이트 시 
~~여유 slot이 2개 있으므로 *yaml파일에서 수정*해서 사용가능~~  
~~더 많은 리스트가 필요할 경우에는 아래와 같이 소스코드를 수정해 준다~~

> 더 많은 리스트가 필요 시 yaml에서 계속 무한으로 만들 수 있음. 단, yaml 파라미터 형태는 유지해야한다.  
list/example.yaml 를 참고

1. qml 에서 추가 시
~~ComboBox {~~
    ~~model: [ "list1", "list2", "list3", "list4", "new...list"]~~
~~}~~

> 위와 마찬가지로 cpp소스 코드도 수정해야 했으나 버전 2.7 부터 list의 yaml파일만 수정해서 사용할 수 있게 수정   



## 버전 history
version 1.0
- sh script 

version 2.0
- c++, Qt, qtml

version 2.1
- vector와 unordered_map을 이용한 방식

version 2.2
- yaml파일 이용- 입력, 수정, 삭제 지원
- unordered_map 사용 안함, 대신 struct 사용

version 2.3
- list 디렉토리 사용 symlink 유무로 확인 가능하게 함

version 2.4
- yaml 파일 업데이트, 여유분으로 2개 더 추가

version 2.5
- RobotCom 클래스를 이용해서 yaml 정보를 하나씩 넣어줬던 방식에서 (여러개의 instances)  
  unordered_map 형태로 만들어서 키, 벨류 형태로 조합 및 사용

version 2.6  
- 처음 실행 시 터미널이 실행 안되는 버그 수정

version 2.7
- QML ComboBox 에서 하드코딩 후에 빌드해야 했으나, yaml 파일에서 읽은 리스트를 사용할 수 있게 변경  
- list/list.yaml 파일에서 첫 번째 파라미터들을 가져온다.