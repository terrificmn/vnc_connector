#ifndef SSHHELPER_H
#define SSHHELPER_H

#include <QObject>
#include <QDebug>
#include <QProcess>
#include <QString>
#include <QClipboard>
#include <QGuiApplication>
#include <QStringList>

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <thread>
#include <algorithm>
#include <yaml-cpp/yaml.h>
#include <filesystem>


class SshHelper : public QObject {
    Q_OBJECT
    // 여기는 함수들 포함해서 지정 (c++에서 QML 쪽으로 보내는 것들을 정의한다고 생각하면 됨)
    // Q_PROPERTY(<함수의리턴타입> <함수이름> READ <함수이름> NOTIFY <signal>)   ---- 여기에서 함수는 현재 클래스의 c++ 함수
    // 프로그램 중 emit 시그널() (NOTIFY) 을 보냈을 경우, 여기에 정의한 함수가 실행이 되게 된다.
    Q_PROPERTY(QString textForPaste READ textForPaste NOTIFY textChanged)
    Q_PROPERTY(QString textHelpForPaste READ textHelpForPaste NOTIFY textChanged)
    Q_PROPERTY(bool isNextBtnEnabled READ isNextBtnEnabled NOTIFY buttonClicked)
    Q_PROPERTY(QStringList readListName READ readListName NOTIFY readListOnce)
    Q_PROPERTY(bool changeBtnText READ changeBtnText NOTIFY sigVncviewerButton);


    // 반대로 QML쪽에서는 C++의 slots함수를 통해서 호출하게 됨

    // Q_PROPERTY(WRITE sshConnector NOTIFY textChanged)
    // Q_PROPERTY(WRITE sshConnector NOTIFY textChanged)

private: 
    std::string user_name;
    bool is_button_clicked = false;
    bool is_next_btn_enabled = false;
    bool is_next_btn_ready = false;
    bool is_tunnelling_enabled = false;
    std::string abs_project_path;
    std::string yaml_path;
    int selected_robot_num = 0;

    std::unordered_map<std::string, std::string> user_map;
    std::vector<std::string> v_user; 

    QProcess *myProcess = nullptr;
    int is_first_cmd_copied = false;
    bool next_btn_to_change = false;

public:
    explicit SshHelper(QObject *parent = nullptr, QProcess *myProcess = nullptr);
    void AssignPath(bool is_dev = true);
    void addUserMap(std::string& com_name, std::string& com_id, std::string& com_ip, std::string& com_port, std::string& com_ssh);
    std::string concatenatedStr(int type_=0);
    
    /// QML 에서 호출되서 사용되는 함수들 - cpp에서 signal함수 호출 후 qml에서 각 함수들 호출해서 값을 받아감. (signals와 같이 사용됨)
    /// QML에서 사용할 때에는 () 없이 사용 예) sshHelper.textForPaste  (SshHelper인스턴스)
    QString textForPaste();
    QString textHelpForPaste();
    QStringList readListName();
    bool changeBtnText();

    bool isNextBtnEnabled(); // 안되는 듯..
    void sshTerminalOpen(int type);
    void setSelectRobotNum(int cbbox_index_);
    bool loadYaml();
    std::string selectComName();

public slots:
    /// qml에서도 사용할 수 있다. 아래 함수들은 Q_PROPERTY 매크로가 설정이 안되어 있지만
    /// Q_PROPERTY 매크로에 설정된 특정 signals(emit)를 트리거 시키면서 연결된 함수를 연속적으로 사용하려고 할 때 사용하게 됨
    void sshConnector(int ckbox_ssh, int ckbox_tunnel, int cbbox_index);
    void sshTunnelConnector(int ckbox_tunnel, int cbbox_index);
    void reset();
    
signals:
    void textChanged();
    void buttonClicked();
    void readListOnce();
    void sigVncviewerButton();

};

#endif // SSHHELPER_H
