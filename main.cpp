#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <ssh_helper.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    QProcess *myProcess = new QProcess(&app);
    SshHelper *sshHelper = new SshHelper(&app, myProcess);
    engine.rootContext()->setContextProperty("sshHelper", sshHelper);

    const QUrl url = QUrl(QStringLiteral("qrc:/vnc_connector/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
