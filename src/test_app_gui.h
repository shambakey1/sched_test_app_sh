/***************************************************************************
 *   Copyright (C) 2009-2010 Virginia Tech Real-Time Systems Lab
 *   Written by Matthew Dellinger
 *   mdelling@vt.edu
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef TEST_APP_GUI_H
#define TEST_APP_GUI_H

/********************************************************************************
** Form generated from reading ui file 'ChronOS_Test_AppDQ4378.ui'
**
** Created: Tue Dec 22 19:33:41 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_2;
    QFrame *frame_5;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_2;
    QFrame *line;
    QGroupBox *groupBox_5;
    QCheckBox *verboseCBox;
    QCheckBox *loggingCBox;
    QCheckBox *excelCBox;
    QCheckBox *gnuCBox;
    QGroupBox *groupBox_6;
    QComboBox *mainCPUCmbBox;
    QLabel *label;
    QFrame *frame_6;
    QFrame *frame_7;
    QLabel *version_label;
    QLabel *versionLabel;
    QTextEdit *textEdit;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QComboBox *schedCmbBox;
    QCheckBox *huaCBox;
    QCheckBox *noDeadlocksCBox;
    QCheckBox *abortIdleCBox;
    QCheckBox *piCBox;
    QGroupBox *groupBox_2;
    QLabel *label_3;
    QCheckBox *lockingCBox;
    QLineEdit *csTBox;
    QComboBox *patternCmbBox;
    QLabel *label_pattern;
    QGroupBox *groupBox_3;
    QLineEdit *usageTBox;
    QLineEdit *execTBox;
    QLabel *label_5;
    QLabel *label_6;
    QCheckBox *batchBox;
    QLabel *label_7;
    QLineEdit *usage2TBox;
    QLabel *label_8;
    QLineEdit *iterationTBox;
    QGroupBox *groupBox_4;
    QLabel *name_label;
    QLabel *label_tasks;
    QLabel *label_usage;
    QLabel *label_period;
    QLabel *label_locks;
    QPushButton *changeTaskSetButton;
    QLabel *tasksLabel;
    QLabel *usageLabel;
    QLabel *hyperPeriodLabel;
    QLabel *locksLabel;
    QLabel *nameLabel;
    QFrame *frame_2;
    QFrame *frame_3;
    QHBoxLayout *horizontalLayout;
    QFrame *frame_4;
    QPushButton *clearButton;
    QPushButton *runButton;
    QMenuBar *menubar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(987, 700);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout_2 = new QHBoxLayout(centralwidget);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        frame_5 = new QFrame(centralwidget);
        frame_5->setObjectName(QString::fromUtf8("frame_5"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame_5->sizePolicy().hasHeightForWidth());
        frame_5->setSizePolicy(sizePolicy);
        frame_5->setMinimumSize(QSize(225, 0));
        frame_5->setFrameShape(QFrame::StyledPanel);
        frame_5->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(frame_5);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label_2 = new QLabel(frame_5);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy1);
        label_2->setMaximumSize(QSize(200, 100));
        label_2->setPixmap(QPixmap(QString::fromUtf8("/usr/local/chronos/chronos.png")));
        label_2->setScaledContents(true);

        verticalLayout_2->addWidget(label_2);

        line = new QFrame(frame_5);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(line);

        groupBox_5 = new QGroupBox(frame_5);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(groupBox_5->sizePolicy().hasHeightForWidth());
        groupBox_5->setSizePolicy(sizePolicy2);
        groupBox_5->setMinimumSize(QSize(0, 105));
        verboseCBox = new QCheckBox(groupBox_5);
        verboseCBox->setObjectName(QString::fromUtf8("verboseCBox"));
        verboseCBox->setGeometry(QRect(10, 20, 141, 20));
        loggingCBox = new QCheckBox(groupBox_5);
        loggingCBox->setObjectName(QString::fromUtf8("loggingCBox"));
        loggingCBox->setGeometry(QRect(10, 80, 141, 20));
        excelCBox = new QCheckBox(groupBox_5);
        excelCBox->setObjectName(QString::fromUtf8("excelCBox"));
        excelCBox->setGeometry(QRect(10, 40, 141, 20));
        gnuCBox = new QCheckBox(groupBox_5);
        gnuCBox->setObjectName(QString::fromUtf8("gnuCBox"));
        gnuCBox->setGeometry(QRect(10, 60, 161, 23));

        verticalLayout_2->addWidget(groupBox_5);

        groupBox_6 = new QGroupBox(frame_5);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        sizePolicy2.setHeightForWidth(groupBox_6->sizePolicy().hasHeightForWidth());
        groupBox_6->setSizePolicy(sizePolicy2);
        groupBox_6->setMinimumSize(QSize(0, 70));
        mainCPUCmbBox = new QComboBox(groupBox_6);
        mainCPUCmbBox->setObjectName(QString::fromUtf8("mainCPUCmbBox"));
        mainCPUCmbBox->setGeometry(QRect(80, 30, 85, 27));
        label = new QLabel(groupBox_6);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 35, 62, 18));

        verticalLayout_2->addWidget(groupBox_6);

        frame_6 = new QFrame(frame_5);
        frame_6->setObjectName(QString::fromUtf8("frame_6"));
        frame_6->setFrameShape(QFrame::NoFrame);
        frame_6->setFrameShadow(QFrame::Raised);

        verticalLayout_2->addWidget(frame_6);

        frame_7 = new QFrame(frame_5);
        frame_7->setObjectName(QString::fromUtf8("frame_7"));
        sizePolicy2.setHeightForWidth(frame_7->sizePolicy().hasHeightForWidth());
        frame_7->setSizePolicy(sizePolicy2);
        frame_7->setMinimumSize(QSize(0, 30));
        frame_7->setFrameShape(QFrame::NoFrame);
        frame_7->setFrameShadow(QFrame::Raised);
        version_label = new QLabel(frame_7);
        version_label->setObjectName(QString::fromUtf8("version_label"));
        version_label->setGeometry(QRect(10, 10, 61, 18));
        versionLabel = new QLabel(frame_7);
        versionLabel->setObjectName(QString::fromUtf8("versionLabel"));
        versionLabel->setGeometry(QRect(70, 10, 62, 18));

        verticalLayout_2->addWidget(frame_7);


        horizontalLayout_2->addWidget(frame_5);

        textEdit = new QTextEdit(centralwidget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(textEdit->sizePolicy().hasHeightForWidth());
        textEdit->setSizePolicy(sizePolicy3);
        textEdit->setReadOnly(true);

        horizontalLayout_2->addWidget(textEdit);

        frame = new QFrame(centralwidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setMinimumSize(QSize(200, 0));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(frame);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(-1, -1, -1, 0);
        groupBox = new QGroupBox(frame);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        sizePolicy2.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy2);
        groupBox->setMinimumSize(QSize(0, 140));
        schedCmbBox = new QComboBox(groupBox);
        schedCmbBox->setObjectName(QString::fromUtf8("schedCmbBox"));
        schedCmbBox->setGeometry(QRect(0, 20, 178, 31));
        huaCBox = new QCheckBox(groupBox);
        huaCBox->setObjectName(QString::fromUtf8("huaCBox"));
        huaCBox->setEnabled(true);
        huaCBox->setGeometry(QRect(0, 80, 178, 23));
        noDeadlocksCBox = new QCheckBox(groupBox);
        noDeadlocksCBox->setObjectName(QString::fromUtf8("noDeadlocksCBox"));
        noDeadlocksCBox->setEnabled(false);
        noDeadlocksCBox->setGeometry(QRect(0, 120, 178, 23));
        abortIdleCBox = new QCheckBox(groupBox);
        abortIdleCBox->setObjectName(QString::fromUtf8("abortIdleCBox"));
        abortIdleCBox->setGeometry(QRect(0, 60, 178, 23));
        abortIdleCBox->setChecked(false);
        piCBox = new QCheckBox(groupBox);
        piCBox->setObjectName(QString::fromUtf8("piCBox"));
        piCBox->setEnabled(false);
        piCBox->setGeometry(QRect(0, 100, 171, 23));

        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(frame);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        sizePolicy2.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy2);
        groupBox_2->setMinimumSize(QSize(0, 112));
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(1, 45, 123, 26));
        lockingCBox = new QCheckBox(groupBox_2);
        lockingCBox->setObjectName(QString::fromUtf8("lockingCBox"));
        lockingCBox->setGeometry(QRect(1, 21, 123, 23));
        csTBox = new QLineEdit(groupBox_2);
        csTBox->setObjectName(QString::fromUtf8("csTBox"));
        csTBox->setGeometry(QRect(125, 45, 53, 26));
        QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(csTBox->sizePolicy().hasHeightForWidth());
        csTBox->setSizePolicy(sizePolicy4);
        patternCmbBox = new QComboBox(groupBox_2);
        patternCmbBox->setObjectName(QString::fromUtf8("patternCmbBox"));
        patternCmbBox->setEnabled(false);
        patternCmbBox->setGeometry(QRect(58, 75, 121, 31));
        label_pattern = new QLabel(groupBox_2);
        label_pattern->setObjectName(QString::fromUtf8("label_pattern"));
        label_pattern->setGeometry(QRect(1, 82, 62, 18));

        verticalLayout->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(frame);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        sizePolicy2.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy2);
        groupBox_3->setMinimumSize(QSize(0, 150));
        usageTBox = new QLineEdit(groupBox_3);
        usageTBox->setObjectName(QString::fromUtf8("usageTBox"));
        usageTBox->setGeometry(QRect(125, 50, 53, 26));
        sizePolicy4.setHeightForWidth(usageTBox->sizePolicy().hasHeightForWidth());
        usageTBox->setSizePolicy(sizePolicy4);
        execTBox = new QLineEdit(groupBox_3);
        execTBox->setObjectName(QString::fromUtf8("execTBox"));
        execTBox->setGeometry(QRect(125, 20, 53, 26));
        sizePolicy4.setHeightForWidth(execTBox->sizePolicy().hasHeightForWidth());
        execTBox->setSizePolicy(sizePolicy4);
        label_5 = new QLabel(groupBox_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(0, 50, 119, 26));
        label_6 = new QLabel(groupBox_3);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(0, 20, 119, 26));
        batchBox = new QCheckBox(groupBox_3);
        batchBox->setObjectName(QString::fromUtf8("batchBox"));
        batchBox->setGeometry(QRect(0, 130, 93, 23));
        label_7 = new QLabel(groupBox_3);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(0, 80, 119, 26));
        usage2TBox = new QLineEdit(groupBox_3);
        usage2TBox->setObjectName(QString::fromUtf8("usage2TBox"));
        usage2TBox->setEnabled(false);
        usage2TBox->setGeometry(QRect(125, 80, 53, 26));
        sizePolicy4.setHeightForWidth(usage2TBox->sizePolicy().hasHeightForWidth());
        usage2TBox->setSizePolicy(sizePolicy4);
        label_8 = new QLabel(groupBox_3);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(0, 110, 119, 26));
        iterationTBox = new QLineEdit(groupBox_3);
        iterationTBox->setObjectName(QString::fromUtf8("iterationTBox"));
        iterationTBox->setEnabled(false);
        iterationTBox->setGeometry(QRect(125, 110, 53, 26));
        sizePolicy4.setHeightForWidth(iterationTBox->sizePolicy().hasHeightForWidth());
        iterationTBox->setSizePolicy(sizePolicy4);

        verticalLayout->addWidget(groupBox_3);

        groupBox_4 = new QGroupBox(frame);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(100);
        sizePolicy5.setHeightForWidth(groupBox_4->sizePolicy().hasHeightForWidth());
        groupBox_4->setSizePolicy(sizePolicy5);
        groupBox_4->setMinimumSize(QSize(0, 110));
        name_label = new QLabel(groupBox_4);
        name_label->setObjectName(QString::fromUtf8("name_label"));
        name_label->setGeometry(QRect(0, 20, 51, 18));
        label_tasks = new QLabel(groupBox_4);
        label_tasks->setObjectName(QString::fromUtf8("label_tasks"));
        label_tasks->setGeometry(QRect(0, 40, 41, 18));
        label_usage = new QLabel(groupBox_4);
        label_usage->setObjectName(QString::fromUtf8("label_usage"));
        label_usage->setGeometry(QRect(0, 60, 51, 18));
        label_period = new QLabel(groupBox_4);
        label_period->setObjectName(QString::fromUtf8("label_period"));
        label_period->setGeometry(QRect(90, 40, 91, 18));
        label_locks = new QLabel(groupBox_4);
        label_locks->setObjectName(QString::fromUtf8("label_locks"));
        label_locks->setGeometry(QRect(90, 60, 41, 18));
        changeTaskSetButton = new QPushButton(groupBox_4);
        changeTaskSetButton->setObjectName(QString::fromUtf8("changeTaskSetButton"));
        changeTaskSetButton->setGeometry(QRect(0, 80, 90, 27));
        tasksLabel = new QLabel(groupBox_4);
        tasksLabel->setObjectName(QString::fromUtf8("tasksLabel"));
        tasksLabel->setGeometry(QRect(50, 40, 31, 18));
        tasksLabel->setContextMenuPolicy(Qt::NoContextMenu);
        tasksLabel->setTextFormat(Qt::PlainText);
        usageLabel = new QLabel(groupBox_4);
        usageLabel->setObjectName(QString::fromUtf8("usageLabel"));
        usageLabel->setGeometry(QRect(50, 60, 29, 18));
        hyperPeriodLabel = new QLabel(groupBox_4);
        hyperPeriodLabel->setObjectName(QString::fromUtf8("hyperPeriodLabel"));
        hyperPeriodLabel->setGeometry(QRect(170, 40, 31, 18));
        locksLabel = new QLabel(groupBox_4);
        locksLabel->setObjectName(QString::fromUtf8("locksLabel"));
        locksLabel->setGeometry(QRect(170, 60, 31, 18));
        nameLabel = new QLabel(groupBox_4);
        nameLabel->setObjectName(QString::fromUtf8("nameLabel"));
        nameLabel->setGeometry(QRect(50, 20, 151, 18));
        nameLabel->setContextMenuPolicy(Qt::NoContextMenu);
        nameLabel->setTextFormat(Qt::PlainText);

        verticalLayout->addWidget(groupBox_4);

        frame_2 = new QFrame(frame);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        QSizePolicy sizePolicy6(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(frame_2->sizePolicy().hasHeightForWidth());
        frame_2->setSizePolicy(sizePolicy6);
        frame_2->setFrameShape(QFrame::NoFrame);
        frame_2->setFrameShadow(QFrame::Raised);

        verticalLayout->addWidget(frame_2);

        frame_3 = new QFrame(frame);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        sizePolicy2.setHeightForWidth(frame_3->sizePolicy().hasHeightForWidth());
        frame_3->setSizePolicy(sizePolicy2);
        frame_3->setMinimumSize(QSize(0, 40));
        frame_3->setFrameShape(QFrame::NoFrame);
        frame_3->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame_3);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 0, -1, 0);
        frame_4 = new QFrame(frame_3);
        frame_4->setObjectName(QString::fromUtf8("frame_4"));
        frame_4->setFrameShape(QFrame::NoFrame);
        frame_4->setFrameShadow(QFrame::Raised);

        horizontalLayout->addWidget(frame_4);

        clearButton = new QPushButton(frame_3);
        clearButton->setObjectName(QString::fromUtf8("clearButton"));

        horizontalLayout->addWidget(clearButton);

        runButton = new QPushButton(frame_3);
        runButton->setObjectName(QString::fromUtf8("runButton"));

        horizontalLayout->addWidget(runButton);


        verticalLayout->addWidget(frame_3);


        horizontalLayout_2->addWidget(frame);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 987, 27));
        MainWindow->setMenuBar(menubar);
        QWidget::setTabOrder(textEdit, verboseCBox);
        QWidget::setTabOrder(verboseCBox, excelCBox);
        QWidget::setTabOrder(excelCBox, gnuCBox);
        QWidget::setTabOrder(gnuCBox, loggingCBox);
        QWidget::setTabOrder(loggingCBox, mainCPUCmbBox);
        QWidget::setTabOrder(mainCPUCmbBox, schedCmbBox);
        QWidget::setTabOrder(schedCmbBox, abortIdleCBox);
        QWidget::setTabOrder(abortIdleCBox, huaCBox);
        QWidget::setTabOrder(huaCBox, piCBox);
        QWidget::setTabOrder(piCBox, noDeadlocksCBox);
        QWidget::setTabOrder(noDeadlocksCBox, lockingCBox);
        QWidget::setTabOrder(lockingCBox, csTBox);
        QWidget::setTabOrder(csTBox, patternCmbBox);
        QWidget::setTabOrder(patternCmbBox, execTBox);
        QWidget::setTabOrder(execTBox, usageTBox);
        QWidget::setTabOrder(usageTBox, usage2TBox);
        QWidget::setTabOrder(usage2TBox, iterationTBox);
        QWidget::setTabOrder(iterationTBox, batchBox);
        QWidget::setTabOrder(batchBox, changeTaskSetButton);
        QWidget::setTabOrder(changeTaskSetButton, clearButton);
        QWidget::setTabOrder(clearButton, runButton);

        retranslateUi(MainWindow);
        QObject::connect(batchBox, SIGNAL(toggled(bool)), iterationTBox, SLOT(setEnabled(bool)));
        QObject::connect(batchBox, SIGNAL(toggled(bool)), usage2TBox, SLOT(setEnabled(bool)));
        QObject::connect(lockingCBox, SIGNAL(toggled(bool)), piCBox, SLOT(setEnabled(bool)));
        QObject::connect(lockingCBox, SIGNAL(toggled(bool)), noDeadlocksCBox, SLOT(setEnabled(bool)));
        QObject::connect(lockingCBox, SIGNAL(toggled(bool)), csTBox, SLOT(setEnabled(bool)));
        QObject::connect(excelCBox, SIGNAL(toggled(bool)), verboseCBox, SLOT(setDisabled(bool)));
        QObject::connect(verboseCBox, SIGNAL(toggled(bool)), excelCBox, SLOT(setDisabled(bool)));
        QObject::connect(excelCBox, SIGNAL(toggled(bool)), gnuCBox, SLOT(setDisabled(bool)));
        QObject::connect(verboseCBox, SIGNAL(toggled(bool)), gnuCBox, SLOT(setDisabled(bool)));
        QObject::connect(gnuCBox, SIGNAL(toggled(bool)), excelCBox, SLOT(setDisabled(bool)));
        QObject::connect(gnuCBox, SIGNAL(toggled(bool)), verboseCBox, SLOT(setDisabled(bool)));
        QObject::connect(lockingCBox, SIGNAL(toggled(bool)), patternCmbBox, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Sched_Test_App", 0, QApplication::UnicodeUTF8));
        label_2->setText(QString());
        groupBox_5->setTitle(QApplication::translate("MainWindow", "Output", 0, QApplication::UnicodeUTF8));
        verboseCBox->setText(QApplication::translate("MainWindow", "Verbose Output", 0, QApplication::UnicodeUTF8));
        loggingCBox->setText(QApplication::translate("MainWindow", "Log to file", 0, QApplication::UnicodeUTF8));
        excelCBox->setText(QApplication::translate("MainWindow", "Output for Excel", 0, QApplication::UnicodeUTF8));
        gnuCBox->setText(QApplication::translate("MainWindow", "Output for Gnuplot", 0, QApplication::UnicodeUTF8));
        groupBox_6->setTitle(QApplication::translate("MainWindow", "CPU Affinity", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Main", 0, QApplication::UnicodeUTF8));
        version_label->setText(QApplication::translate("MainWindow", "Version:", 0, QApplication::UnicodeUTF8));
        versionLabel->setText(QApplication::translate("MainWindow", "1.0.0", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindow", "Scheduling Algorithm", 0, QApplication::UnicodeUTF8));
        huaCBox->setText(QApplication::translate("MainWindow", "Use HUA handlers", 0, QApplication::UnicodeUTF8));
        noDeadlocksCBox->setText(QApplication::translate("MainWindow", "Deadlock prevention", 0, QApplication::UnicodeUTF8));
        abortIdleCBox->setText(QApplication::translate("MainWindow", "Abort tasks on idle", 0, QApplication::UnicodeUTF8));
        piCBox->setText(QApplication::translate("MainWindow", "Use PI", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Locking", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "CS length (%)", 0, QApplication::UnicodeUTF8));
        lockingCBox->setText(QApplication::translate("MainWindow", "Enable locking", 0, QApplication::UnicodeUTF8));
        patternCmbBox->clear();
        patternCmbBox->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Sequential", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Nested", 0, QApplication::UnicodeUTF8)
        );
        label_pattern->setText(QApplication::translate("MainWindow", "Pattern", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("MainWindow", "Run Properties", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "CPU usage (%)", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "Execution time (s)", 0, QApplication::UnicodeUTF8));
        batchBox->setText(QApplication::translate("MainWindow", "Batch Run", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainWindow", "End usage (%)", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MainWindow", "Iteration (%)", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("MainWindow", "Taskset", 0, QApplication::UnicodeUTF8));
        name_label->setText(QApplication::translate("MainWindow", "Name:", 0, QApplication::UnicodeUTF8));
        label_tasks->setText(QApplication::translate("MainWindow", "Tasks:", 0, QApplication::UnicodeUTF8));
        label_usage->setText(QApplication::translate("MainWindow", "Usage:", 0, QApplication::UnicodeUTF8));
        label_period->setText(QApplication::translate("MainWindow", "TS Period:", 0, QApplication::UnicodeUTF8));
        label_locks->setText(QApplication::translate("MainWindow", "Locks:", 0, QApplication::UnicodeUTF8));
        changeTaskSetButton->setText(QApplication::translate("MainWindow", "Change", 0, QApplication::UnicodeUTF8));
        tasksLabel->setText(QString());
        usageLabel->setText(QString());
        hyperPeriodLabel->setText(QString());
        locksLabel->setText(QString());
        nameLabel->setText(QString());
        clearButton->setText(QApplication::translate("MainWindow", "Clear", 0, QApplication::UnicodeUTF8));
        runButton->setText(QApplication::translate("MainWindow", "Run", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(MainWindow);
    } // retranslateUi
};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

#endif
