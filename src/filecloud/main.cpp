
/*
 * CloudVPN
 *
 * This program is a free software: You can redistribute and/or modify it
 * under the terms of GNU GPLv3 license, or any later version of the license.
 * The program is distributed in a good hope it will be useful, but without
 * any warranty - see the aforementioned license for more details.
 * You should have received a copy of the license along with this program;
 * if not, see <http://www.gnu.org/licenses/>.
 */


#include <QApplication>
#include <QPushButton>

int main(int argc, char**argv)
{
	QApplication a(argc,argv);
	QWidget window;
	window.resize(400,600);
	QPushButton hello("i am a stupid\nplaceholder", &window );
	hello.setGeometry(10, 40, 180, 40);

	window.show();
	return a.exec();
}

