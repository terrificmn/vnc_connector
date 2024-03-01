# vnc_connector

Release 버전으로 빌드 후에 원하는 디렉토리를 (예 vnc_connector, 패키지 이름과 같게..)만들어서 이동시켜준다.  
list 디렉토리도 같이 복사해준다 (list/example.yaml 파일도 복사)

```
mkdir ~/vnc_connector
mv ~/qt_ws/build-vnc_connector-vnc_connector-kit버전-Release/appvnc_connector ~/vnc_connector/
cp -r ~/qt_ws/vnc_connector/list ~/vnc_connector/
```

> build 디렉토리는 다를 수 있다

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
기존의 yaml 형식대로 카피해서 사용을 하고, 추가로 자동으로 만들어 주는 것 따위(?)는 개발이 안되어 있으므로   
하나씩 다 넣어줘야 한다  

여유 slot이 2개 있으므로 *yaml파일에서 수정*해서 사용가능

더 많은 리스트가 필요할 경우에는 아래와 같이 소스코드를 수정해 준다

1. qml 에서 추가
```js
ComboBox {
    // .. 생략
    model: [ "list1", "list2", "list3", "list4", "new...list"]
    // 생략...
}
```
> 이 때 qml의 콤보 박스의 이름은 상관 없음, 인덱스로 사용하게 됨

2. SssHelper 클래스의 private 멤버 변수 중 sturct RobotCom 을 추가해준다 
예:
```cpp
private:
// 생략...
    RobotCom etc_user3;
```

3. SssHelper 클래스의 `loadYaml()` 메소드에 `else if` 를 추가한다
예:
```cpp
loadYaml() { // 메소드 중에
    if(size > 9)  // 사이즈 변경 - 현재는 8, 추가할 시 yaml 파일의 요소 만큼 추가해서 비교

    /// 생략...
     else if(count == 8) {  // 카운트 변경
        // 조건 추가
        ptr_robot_com = &this->new_com;  // private 멤버로 추가한 struct 변수를 추가해준다
    }

    // 생략
}
```

4. SshHelper 클래스의 `selectRobotPtr()` 메소드에 `else if` 구문 추가
```cpp
selectRobotPtr() {
    /// 생략...
      } else if(this->selected_robot_num == 8) {
        ptr_robot_com = &this->new_com; // private struct으로 추가된 변수
}
```

> 구조가 조금 비슷한게 많아서 수정이 필요할 듯 하지만, 그냥 이대로 사용



## 버전
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
