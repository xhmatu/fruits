/*
#############################################################################
# COMP.CS.110 Programming 2: Autumn 2020                                    #
# Project4: Match3 (GUI project)                                            #
# File: main.cpp                                                            #
# Description: instruction.txt-tiedostossa lukee kaikki oleellinen.         #
#                                                                           #
#                                                                           #
#        Ohjelman kirjoittaja                                               #
#            * Nimi: Max Turunen                                            #
#            * Opiskelijanumero: 95284                                      #
#            * Käyttäjätunnus: xhmatu (Git-repositorion hakemistonimi. )    #
#            * E-Mail: max.turunen@tuni.fi                                  #
#            * Palautteen kieli (fi/en): fi                                 #
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
