
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
#include <QGroupBox>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QPushButton>
#include <QSplitter>
#include <QTreeView>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QLabel>

int main (int argc, char**argv)
{
	QApplication app (argc, argv);
	QMainWindow window;

	window.resize (800, 600);

	QSplitter h_layout (Qt::Horizontal, &window);
	window.setCentralWidget (&h_layout);

	QSplitter leftbar (Qt::Vertical, &h_layout);
	QSplitter rightbar (Qt::Vertical, &h_layout);

	QGroupBox sharegroup ("shares", &leftbar);
	QGroupBox downloadgroup ("downloads", &leftbar);
	QGroupBox searchgroup ("search results", &rightbar);
	QGroupBox settingroup ("settings", &rightbar);

	QVBoxLayout se_l;
	QVBoxLayout do_l;
	QVBoxLayout sh_l;
	QFormLayout settingslayout;

	QTreeView searchview (&searchgroup);
	QTreeView downloadview (&downloadgroup);
	QTreeView shareview (&sharegroup);

	se_l.addWidget (&searchview);
	do_l.addWidget (&downloadview);
	sh_l.addWidget (&shareview);

	searchgroup.setLayout (&se_l);
	downloadgroup.setLayout (&do_l);
	sharegroup.setLayout (&sh_l);
	settingroup.setLayout (&settingslayout);

	window.menuBar()->addMenu ("blesmrt");
	window.menuBar()->addMenu ("blesmrt");
	window.menuBar()->addMenu ("blesmrt");
	window.menuBar()->addMenu ("blesmrt");

	window.show();
	return app.exec();
}

