#include "ssh_helper.h"

SshHelper::SshHelper(QObject *parent, QProcess *myProcess) : QObject(parent) {
    qDebug() << "Init!";
    this->myProcess = myProcess;
    this->AssignPath(false); // release 일 경우는 false, debug 일 경우 true (기본:true)
    if(!this->loadYaml()) {
        std::cout << "failed load a config. Program will not work properly." << std::endl;
    }

    // print test
    // std::cout << this->s100.id << " " << this->s100.ip << " " << this->s100.port << " " << this->s100.ssh << std::endl;
    // std::cout << this->p150.id << " " << this->p150.ip << " " << this->p150.port << " " << this->p150.ssh << std::endl;
    // std::cout << this->p150_velo.id << " " << this->p150_velo.ip << " " << this->p150_velo.port << " " << this->p150_velo.ssh << std::endl;
    // std::cout << this->v100_new.id << " " << this->v100_new.ip << " " << this->v100_new.port << " " << this->v100_new.ssh << std::endl;
    // std::cout << "end" << std::endl;
    
}


/**
 * @brief find path for loading yaml. call the this function passing by an argument as false when needed to release 
 *  build-.... 디렉토리가 현재 디렉토리로 인식 및 사용되므로, 현재 프로젝트내에 있는 list 디렉토리에 접근하기 위해서
 *  development 일 때에는 현재 디렉토리에 실행, release 빌드 후 실제로 파일이 다른 곳으로 이동한다는 가정하에...
 *  실제는 release하면 실행 파일이 있는 부모(현재) 디렉토리에서 실행하므로 그곳에서 yaml파일 읽음
 * @param is_dev bool: default true. 
 */
void SshHelper::AssignPath(bool is_dev) {
    // project path 
    std::filesystem::path p = std::filesystem::current_path();
    if(is_dev) { //default is true
        std::string tmp_abs_dir = std::filesystem::absolute(p.parent_path()); //dev 일 경우만 사용
        this->abs_project_path = tmp_abs_dir + "/vnc_connector"; //project (//dev 
    } else {
        /// symlink 일 경우에 확인: release의 경우 symlink을 해서 실행할 경우에는 symlink가 있는 디렉토리를 검색해서 실제yaml파일을 찾을 수 없게됨
        std::array<const char*, 2> a_sym_path_execute_names = { "/usr/local/bin/vnc_connector", "/usr/bin/vnc_connector" };
        for(auto sym_path_execute_name : a_sym_path_execute_names) {
            if(std::filesystem::is_symlink(sym_path_execute_name)) {
                // std::cout << "read symlink: " << std::filesystem::read_symlink(sym_path_execute_name) << std::endl;
                std::filesystem::path origin_path = std::filesystem::absolute(std::filesystem::read_symlink(sym_path_execute_name));
                this->abs_project_path = std::filesystem::absolute(origin_path.parent_path());
                std::cout << "symlink found" << std::endl;
                break;
            } else { 
                std::cout << "not symlink" << std::endl;
            }
        }

        // this->abs_project_path = p;
        // this->abs_project_path = std::filesystem::absolute(p); //project (it can be changed when release)
    }
    // std::cout << "Current path: " << std::filesystem::current_path() << std::endl;
    // std::cout << std::filesystem::absolute(p.parent_path()) << std::endl;

    this->yaml_path = this->abs_project_path + "/list/list.yaml";
    // std::cout << "yaml path: " << this->yaml_path << std::endl;
}


/**
 * @brief load a yaml file
 * 
 * @return true 
 * @return false 
 */
bool SshHelper::loadYaml() {
    try {
        YAML::Node yaml_load = YAML::LoadFile(this->yaml_path);
        // int size = yaml_load.size(); // size 제한 없음
        
        for(YAML::const_iterator it = yaml_load.begin(); it!=yaml_load.end();++it) {
            YAML::Node com_node_name = it->first;
            YAML::Node com = it->second;

            // std::cout << "com name " << com_name << std::endl;
            // std::cout << com["id"] << std::endl;
            // std::cout << com["ip"] << std::endl;
            // std::cout << com["port"] << std::endl;
            // std::cout << com["ssh"] << std::endl;

            std::string com_name = com_node_name.as<std::string>();
            std::string com_id = com["id"].as<std::string>();
            std::string com_ip = com["ip"].as<std::string>();
            std::string com_port = com["port"].as<std::string>();
            std::string com_ssh = com["ssh"].as<std::string>();

            this->addUserMap(com_name, com_id, com_ip, com_port, com_ssh);
        }

        // test print
        // for(auto map : this->user_map) {
        //     std::cout << map.first << " :" << map.second << std::endl;
        // }

    } catch(...) {
        std::cout << "Opening yaml file failure. Error occured..." << std::endl;
        return false;

    }
    // std::cout << yaml_load["id"].as<int>() << std::endl;
    // std::cout << yaml_load["is_id_ever_get"].as<std::string>() << std::endl;
    std::cout << "a config file loaded successfully" << std::endl;
    emit readListOnce();
    return true;
}


void SshHelper::addUserMap(std::string& com_name, std::string& com_id, std::string& com_ip, std::string& com_port, std::string& com_ssh) {
    /// 대표이름으로 사용할 vector, yaml의 각 대표 이름
    this->v_user.push_back(com_name);

    //// user_map convetion; {v_user}_{id , ip, port, or ssh}
    std::string con_id_str = com_name + "_id";
    std::string con_ip_str = com_name + "_ip";
    std::string con_port_str = com_name + "_port";
    std::string con_ssh_str = com_name + "_ssh";

    this->user_map[con_id_str] = com_id;
    this->user_map[con_ip_str] = com_ip;
    this->user_map[con_port_str] = com_port;
    this->user_map[con_ssh_str] = com_ssh;

    qDebug("mapping done!");
}


/// @brief called by QML, then trigger the signal and the connected function 
/// button click(QML) -> this function -> emit (signals) buttonClicked() -> connected function (READ) call
///
/// for example: button clicked(QML) -> sshConnector() -> emit textChanged() -> 
/// -> 여기서 시글널 발생시 헤더파일에 Q_PROPERTY() 매크로에 의해 정의된 textHelpForPaste() 함수가 실행됨
/// @param ckbox_ssh 
/// @param ckbox_tunnel 
/// @param cbbox_index 
void SshHelper::sshConnector(int ckbox_ssh, int ckbox_tunnel, int cbbox_index) {
    this->is_button_clicked = true;

    if(ckbox_ssh == 0 && ckbox_tunnel == 0) {
        qDebug() << "nothing happen";
        this->is_button_clicked = false;
        this->is_next_btn_ready = false;
        this->is_tunnelling_enabled = false;
        emit textChanged();
        emit buttonClicked();
        return;
    }

    this->setSelectRobotNum(cbbox_index);

    if(ckbox_ssh == 0 && ckbox_tunnel == 2) {
        this->is_button_clicked = false;
        this->is_tunnelling_enabled = true;
        qDebug() << "ssh tunnel checked: enabled";
        this->sshTerminalOpen(1); // 1 for ssh tunnel 
        // this->is_next_btn_ready = false;
        emit buttonClicked();
        return;
    }
    

    if(ckbox_ssh == 2 && ckbox_tunnel == 0) {
        qDebug() << "ssh checked: enabled";
        this->is_tunnelling_enabled = false;
        this->sshTerminalOpen(0); // 0 for ssh

    } else if(ckbox_ssh == 2 && ckbox_tunnel == 2) {
        qDebug() << "both ssh and tunnel checked: enabled";
        this->is_tunnelling_enabled = true;
        this->sshTerminalOpen(0); // 0 for ssh  
    } else {
        qDebug() << "ssh unchecked: disabled";
    }

    // is_next_btn_ready will be true so that NEXT btn appears
    this->is_next_btn_ready = true;

    emit textChanged(); // emit 되는 순간에 qml에서 신호를 받는다 

    qDebug() << "sshConnector executed!";
    this->is_next_btn_enabled = true;
    emit buttonClicked();

}

// return v_user의 문자열, yaml파일의 대표 name이 들어가 있음.
// unordered_map 에서 인덱스 대신에 사용한다. 에: com_name1 ---> 이후 com_name1_ip, com_name1_ssh 등의 key 값으로 사용이된다.
std::string SshHelper::selectComName() {
    return this->v_user[this->selected_robot_num];
}

/**
 * @brief it creates strings which is going to use in a terminal to execute 
 * type_ == 0 : ssh connection command,  type_ == 1: vncserver command,  type_ == 2: vncviewer command
 * @param type_ 
 * @return std::string 
 */
std::string SshHelper::concatenatedStr(int type_) {
    std::string concanated = "ssh ";
    // std::cout << "selectComName: " << this->selectComName() << std::endl;
    std::string user_map_prefix = this->selectComName();
    if(type_ == 0) {
        if(this->user_map[user_map_prefix + "_ssh"] == "22") {
            concanated += this->user_map[user_map_prefix + "_id"] + "@" + this->user_map[user_map_prefix + "_ip"];
        } else {
            concanated += this->user_map[user_map_prefix + "_id"] + "@" + this->user_map[user_map_prefix + "_ip"] + " -p " + this->user_map[user_map_prefix + "_ssh"];
        }

    } else if(type_ == 1) {
        if(this->is_tunnelling_enabled) {
            /// 일단 내부port를 입력 받은 것은 아직 없으므로, 기본적으로 5901를 사용 안하고 사용한다고 하면 5902 포트라고 고정
            std::string default_port = "5901";
            if(this->user_map[user_map_prefix +"_port"] != "5901") {
                /// TODO: 추후 내부포트 입력 받는 것 추가하기
                default_port = "5902";
            }
            if(this->user_map[user_map_prefix + "_ssh"] == "22") {
                concanated += "-L " + this->user_map[user_map_prefix + "_port"] + ":127.0.0.1:" + default_port + " -N -f -l " + this->user_map[user_map_prefix + "_id"] + " " + this->user_map[user_map_prefix + "_ip"];
            } else {
                concanated += "-p " + this->user_map[user_map_prefix + "_ssh"] + " -L " + this->user_map[user_map_prefix + "_port"] + ":127.0.0.1:" + default_port + " -N -f -l " + this->user_map[user_map_prefix + "_id"] + " " + this->user_map[user_map_prefix + "_ip"];
            }

        } else {
            concanated = "vncserver :2 -localhost no";
        }

    } else { // 2, NEXT로 이미 버튼이 한번 눌린 상태에서 viewer 관련 cmd만 출력
        if(this->is_tunnelling_enabled) {
            concanated = "vncviewer localhost:" + this->user_map[user_map_prefix+"_port"] + " &";
        } else {
            concanated = "vncviewer " + this->user_map[user_map_prefix + "_ip"] + ":" + this->user_map[user_map_prefix+"_port"] + " &";
        }
    }

    // std::cout << "!!!!concanated: " << concanated << " !!!" << std::endl; // test print
    return concanated;

}


QString SshHelper::textForPaste() {
    std::string concated_;
    if(!this->is_button_clicked) {
        concated_ = "Please select one from combo lists first";
    } else {
        concated_ = "Successfully copied into clipboard!!. \n1. Now! paste it in the terminal that just pop-up opended.\n2. copy below this and paste it once again after you log-in to the remote pc";
        
    }
    return QString::fromStdString(concated_);

}


QString SshHelper::textHelpForPaste() {
    if(!this->is_button_clicked) {
        return "Copy and paste the message which will be appeared shortly\nafter you click the OK button";
    } else {
        if(this->is_next_btn_enabled) {
            std::string user_map_prefix = this->selectComName();
            std::string tmp_str = "vncviewer localhost:" + this->user_map[user_map_prefix+"_port"] + " &";
            return QString::fromStdString(tmp_str);
        }
        return "vncserver :1 -localhost -geometry 1024x768 -depth 24";
    }
}


bool SshHelper::isNextBtnEnabled() {
    if(!this->is_next_btn_ready) {
        return false;
    }
    return this->is_next_btn_enabled ;
}

/// @brief  change NEXT btn's text. connected signals is sigVncviewerButton()
/// @return 
bool SshHelper::changeBtnText() {
    if(!this->is_first_cmd_copied) {
        this->next_btn_to_change = false;
    } else {
        this->next_btn_to_change = true;
    }

    return this->next_btn_to_change;
}

/**
 * @brief value of combo list from QML, selected_robot_num variable set
 * 
 * @param cbbox_index_ 
 */
void SshHelper::setSelectRobotNum(int cbbox_index_) {
    this->selected_robot_num = cbbox_index_;
}

void SshHelper::sshTerminalOpen(int type) {
    qDebug() << "sshTunnel start!!";

    std::string result_str = this->concatenatedStr(type);  //type 1 : ssh turnnel
    std::cout << result_str << std::endl;

    QString clip_txt = QString::fromStdString(result_str); // std::string 이라 변환!
    QClipboard *clipboard =  QGuiApplication::clipboard();
    clipboard->setText(clip_txt);
    qDebug() << "copied on clipboard";

    if(type == 2) {
        std::cout << "not open a new terminal\n";
        return;
    }

    if(is_tunnelling_enabled || type == 0) { // tennelling checkout 박스 없을 경우에는 프로그램 실행 안함
        QString program = "terminator";
        myProcess->start(program);
        qDebug() << "second terminal opened!";
    }
}

void SshHelper::sshTunnelConnector(int ckbox_tunnel, int cbbox_index) {
    if(!this->is_next_btn_ready) {
        qDebug("please checkbox first1");
        return;
    }

    // is_next_btn_ready will be true, if unchecked, then excuted without the ssh tunneling
    // if(ckbox_tunnel != 2) {
    //     qDebug("please checkbox first2");
    //     return;
    // }

    if(myProcess == nullptr) {
        qDebug("process not started");
    }

    emit textChanged();

    this->setSelectRobotNum(cbbox_index);

    this->textHelpForPaste();
    std::cout << "please close the terminal" << std::endl;

    // it does not work in the Rocky
    if(myProcess->waitForFinished(30000)) {
        std::cout << "it's finished" << std::endl;
        emit textChanged();
    }

    // NEXT 버튼으로 한번 수행 했을 경우 더 이상 새로운 창 안 띄움
    if(!this->is_first_cmd_copied) {
        this->sshTerminalOpen(1); // 1 for ssh tunnel 
        this->is_first_cmd_copied = true;
        this->sigVncviewerButton();

    } else {
        this->sshTerminalOpen(2); // without terminal open, type 2는 현재 마지막 클릭이라고 가정 -viewer 관련 복사가 될 수 있게 처리
    }

}

void SshHelper::reset() { 
    this->is_button_clicked = false;
    this->is_next_btn_enabled = false;
    this->is_next_btn_ready = false;
    this->is_first_cmd_copied = false;
    this->next_btn_to_change = false;

    emit textChanged();
    emit buttonClicked();
    emit sigVncviewerButton();
}

QStringList SshHelper::readListName() {
    //  "s100", "p150", "p150-velo", "v100", "win_office", "mecanum", "etc1", "etc2"
    QStringList list;
    
    for(int i=0; i < this->v_user.size(); ++i) {
        list << QString::fromStdString(this->v_user[i]);
        // list.push_back(QString::fromStdString(this->v_user[i]));
        // qDebug() << QString::fromStdString(this->v_user[i]);
    }
    qDebug() << list;
    return list;
}
