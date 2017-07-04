
#include <QDebug>
#include <QGuiApplication>

void example1();
void example2();

int main(int argc, char *argv[])
{
	Q_UNUSED(argc);
	Q_UNUSED(argv);

	qDebug() << "multi-index-examples";
	example1();
	example2();
}
