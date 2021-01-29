/*
#############################################################################
#                                                                           #
# Project: fruits (GUI project)                                             #
# File: main.cpp                                                            #
# Description: instruction.txt-tiedostossa lukee kaikki oleellinen.         #
#                                                                           #
#                                                                           #
#        Ohjelman kirjoittaja                                               #
#            Max Turunen                                                    #
#                                                                           #
#############################################################################
*/

#include "mainwindow.hh"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
