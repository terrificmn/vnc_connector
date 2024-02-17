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
        std::string sym_path_execute_name = "/usr/bin/vnc_connector";
        // std::cout << "sym path: " << sym_path_execute_name << std::endl;
        if(std::filesystem::is_symlink(sym_path_execute_name)) {
            // std::cout << "read symlink: " << std::filesystem::read_symlink(sym_path_execute_name) << std::endl;
            std::filesystem::path origin_path = std::filesystem::absolute(std::filesystem::read_symlink(sym_path_execute_name));
            this->abs_project_path = std::filesystem::absolute(origin_path.parent_path());
        } else { 
            std::cout << "not symlink" << std::endl;
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
        int size = yaml_load.size();
        int count=0;
        if(size > 8) { // 현재는 리스트 5개  --> 8개
            std::cout << "error: a one group in the yaml file found that is exceeded to handle." << std::endl;
            return false;
        }

        for(YAML::const_iterator it = yaml_load.begin(); it!=yaml_load.end();++it) {
            YAML::Node com = it->second;
            // std::cout << com["id"] << std::endl;
            // std::cout << com["ip"] << std::endl;
            // std::cout << com["port"] << std::endl;
            // std::cout << com["ssh"] << std::endl;

            std::string com_id = com["id"].as<std::string>();
            std::string com_ip = com["ip"].as<std::string>();
            std::string com_port = com["port"].as<std::string>();
            std::string com_ssh = com["ssh"].as<std::string>();
            
            RobotCom* ptr_robot_com = nullptr;
            if(count == 0) {
                ptr_robot_com = &this->s100;
            } else if(count == 1) {
                ptr_robot_com = &this->p150;
            } else if(count == 2) {
                ptr_robot_com = &this->p150_velo;
            } else if(count == 3) {
                ptr_robot_com = &this->v100_new;
            } else if(count == 4) {
                ptr_robot_com = &this->win_office;
            } else if(count == 5) {
                ptr_robot_com = &this->mecanum;
            } else if(count == 6) {
                ptr_robot_com = &this->etc_user1;
            } else if(count == 7) {
                ptr_robot_com = &this->etc_user2;
            } 
            
            this->makeStructValues(com_id, com_ip, com_port, com_ssh, ptr_robot_com);
            count++;
        }

    } catch(...) {
        std::cout << "Opening yaml file failure. Error occured..." << std::endl;
        return false;

    }
    // std::cout << yaml_load["id"].as<int>() << std::endl;
    // std::cout << yaml_load["is_id_ever_get"].as<std::string>() << std::endl;
    std::cout << "a config file loaded successfully" << std::endl;
    return true;
}

/**
 * @brief struct RobotCom 포인터로 받아서 처리, yaml에서 읽은 데이터를 하나씩 할당
 * 
 * @param com_id 
 * @param com_ip 
 * @param com_port 
 * @param com_ssh 
 * @param robot_com pointer of struct RobotCom
 */
void SshHelper::makeStructValues(std::string com_id, std::string com_ip, std::string com_port, std::string com_ssh, RobotCom* robot_com) {
        robot_com->id = com_id;
        robot_com->ip = com_ip;
        robot_com->port = com_port;
        robot_com->ssh = com_ssh;
}


void SshHelper::sshConnector(int ckbox_ssh, int ckbox_tunnel, int cbbox_index) {
    this->is_button_clicked = true;

    if(ckbox_ssh == 0 && ckbox_tunnel == 0) {
        qDebug() << "nothing happen";
        this->is_button_clicked = false;
        this->is_next_btn_ready = false;
        emit textChanged();
        emit buttonClicked();
        return;
    }

    this->setSelectRobotNum(cbbox_index);

    if(ckbox_ssh == 0 && ckbox_tunnel == 2) {
        this->sshTerminalOpen(1); // 1 for ssh tunnel 
        this->is_button_clicked = false;
        // this->is_next_btn_ready = false;
        emit buttonClicked();
        return;
    }
    

    if(ckbox_ssh == 2) {
        qDebug() << "ssh checked: enabled";
        this->sshTerminalOpen(0); // 0 for ssh  

    } else {
        qDebug() << "ssh unchecked: disabled";
    }

    if(ckbox_tunnel == 2) {
        qDebug() << "ssh tunnel checked: enabled";
        this->is_next_btn_ready = true;

    } else {
        qDebug() << "ssh tunnel unchecked: disabled";
        this->is_next_btn_ready = false;
    }

    emit textChanged(); // emit 되는 순간에 qml에서 신호를 받는다 

    qDebug() << "sshConnector executed!";
    this->is_next_btn_enabled = true;
    emit buttonClicked();

}

RobotCom* SshHelper::selectRobotPtr() {
    RobotCom* ptr_robot_com = nullptr;
    if(this->selected_robot_num == 0) {
        ptr_robot_com = &this->s100;
    } else if(this->selected_robot_num == 1) {
        ptr_robot_com = &this->p150;
    } else if(this->selected_robot_num == 2) {
        ptr_robot_com = &this->p150_velo;
    } else if(this->selected_robot_num == 3) {
        ptr_robot_com = &this->v100_new;
    } else if(this->selected_robot_num == 4) {
        ptr_robot_com = &this->win_office;
    } else if(this->selected_robot_num == 5) {
        ptr_robot_com = &this->mecanum;
    } else if(this->selected_robot_num == 6) {
        ptr_robot_com = &this->etc_user1;
    } else if(this->selected_robot_num == 7) {
        ptr_robot_com = &this->etc_user2;
    }
    return ptr_robot_com;
}

/**
 * @brief it creates strings which is going to use in a terminal to execute 
 * 
 * @param type_ 
 * @return std::string 
 */
std::string SshHelper::concatenatedStr(int type_) {
    RobotCom* ptr_robot_com = this->selectRobotPtr();

    std::string concanated = "ssh ";
    if(type_ == 0) {
        if(ptr_robot_com->ssh == "22") {
            concanated += ptr_robot_com->id + "@" + ptr_robot_com->ip;
        } else {
            concanated += ptr_robot_com->id + "@" + ptr_robot_com->ip + " -p " + ptr_robot_com->ssh;
        }

    } else {
        if(ptr_robot_com->ssh == "22") {
            concanated += "-L " + ptr_robot_com->port + ":127.0.0.1:5901 -N -f -l " + ptr_robot_com->id + " " + ptr_robot_com->ip;
        } else {
            concanated += "-p " + ptr_robot_com->ssh + " -L " + ptr_robot_com->port + ":127.0.0.1:5901 -N -f -l " + ptr_robot_com->id + " " + ptr_robot_com->ip;
        }
    }
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
            RobotCom* ptr_robot_com = this->selectRobotPtr();
            std::string tmp_str = "vncviewer localhost:" + ptr_robot_com->port + " &";
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

/**
 * @brief value of combo list from QML, selected_robot_num variable set
 * 
 * @param cbbox_index_ 
 */
void SshHelper::setSelectRobotNum(int cbbox_index_) {
//    if(cbbox_index_ > 7) { // total count of current combobox comparison
//        std::cout << "combo box index is wrong. setSelectRobotNum()" << std::endl;
//    }
    // just assign cbbox_index_ to selected_robot_num as simple
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

    QString program = "terminator";
    myProcess->start(program);
    qDebug() << "second terminal opened!";
}

void SshHelper::sshTunnelConnector(int ckbox_tunnel, int cbbox_index) {
    if(!this->is_next_btn_ready) {
        qDebug("please checkbox first1");
        return;
    }

    if(ckbox_tunnel != 2) {
        qDebug("please checkbox first2");
        return;
    }

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

    this->sshTerminalOpen(1); // 1 for ssh tunnel 

}

void SshHelper::reset() { 
    this->is_button_clicked = false;
    this->is_next_btn_enabled = false;
    this->is_next_btn_ready = false;
    
    emit textChanged();
    emit buttonClicked();
}
