import QtQuick
import QtQuick.Window
import QtQuick.Controls

Window {
    width: 540
    height: 400
    visible: true
    title: qsTr("VNC Connection")

    Column {
        anchors.fill: parent

        Rectangle {
            width: parent.width
            height: parent.height * 0.4
            color: "lightgreen"

            Item {
                anchors.fill: parent
                anchors.margins: 10

                Label {
                    id: label_choose
                    text: "Choose one"
                    font.pixelSize: 18
                    color: "steelblue"
                }

                ComboBox {
                    id: cbbox_user
                    width: 200
                    model: sshHelper.readListName //get QStringList from c++ function invoked by signal readListOnce
                    anchors.top: label_choose.bottom
                }

                CheckBox {
                    id: ckbox_ssh
                    checked: true
                    text: qsTr("ssh connection to enable vncserver")
                    anchors.top: cbbox_user.bottom
                }

                CheckBox {
                    id: ckbox_ssh_tunnel
                    checked: true
                    text: qsTr("ssh tunnelling through localhost")
                    anchors.top: ckbox_ssh.bottom
                }
            }
        }

        Rectangle {
            width: parent.width
            height: parent.height * 0.6
            color: "lightblue"
            
            Item {
                anchors.fill: parent
                anchors.margins: 10

                Button {
                    id: button
                    text: qsTr("OK")
                    // anchors.centerIn: column2
                    //         //it can be work too
                    // background: Rectangle {
                    //     color: parent.down ? "#bbbbbb" : (parent.hovered ? "#d6d6d6" : "#f6f6f6")
                    // }
                    palette {
                        button: "green"
                    }

                    onClicked: {
                        console.log("button clicked")
                        // console.log(cbbox_user.currentText)
                        // console.log(cbbox_user.currentIndex)
                        // console.log(ckbox_ssh.checkState)
                        sshHelper.sshConnector(ckbox_ssh.checkState, ckbox_ssh_tunnel.checkState, cbbox_user.currentIndex)

                    }

                }

                Button {
                    id: next_button
                    text: sshHelper.changeBtnText ? qsTr("VNCViewer") : qsTr("NEXT")
                    anchors.left: button.right
                    anchors.leftMargin: 10

                    enabled: sshHelper.isNextBtnEnabled ? true : false

                    palette {
                        button: sshHelper.isNextBtnEnabled ? "deepskyblue" : "gray"
                    }

                    onClicked: {
                        console.log("Next button clicked")
                        // console.log(cbbox_user.currentText)
                        // console.log(cbbox_user.currentIndex)
                        // console.log(ckbox_ssh_tunnel.checkState)
                        sshHelper.sshTunnelConnector(ckbox_ssh_tunnel.checkState, cbbox_user.currentIndex)
                    }
                }

                TextEdit {
                    id: text_paste
                    // horizontalAlignment: Text.AlignCenter
                    anchors.top: next_button.bottom
                    anchors.topMargin: 10
                    // anchors.verticalCenter: parent.verticalCenter
                    width: 150
                    height: 25
                    readOnly: true
                    selectByMouse: true
                    text: sshHelper.textForPaste  // 에러발생 SshHelper 헤더파일에서 QString 지정안되서 그런듯 하다
                }

                TextEdit {
                    id: text_help_msg
                    anchors.top: text_paste.bottom
                    anchors.topMargin: 30
                    horizontalAlignment: Text.AlignCenter
                    // anchors.top: text_paste.bottom
                    // anchors.topMargin: 30
                    // anchors.verticalCenter: parent.verticalCenter
                    width: 150
                    height: 25
                    readOnly: true
                    selectByMouse: true
                    text: sshHelper.textHelpForPaste
                }

                Button {
                    id: reset
                    anchors.top: text_help_msg.bottom
                    anchors.topMargin: 20
                    text: qsTr("refresh")
                    onClicked: {
                        sshHelper.reset()
                    }
                }

                TextEdit {
                    id: text_version
                    anchors.top: reset.bottom
                    anchors.topMargin: 30
                    text: qsTr("version: 2.9")
                }
            }
        }
    }

}
