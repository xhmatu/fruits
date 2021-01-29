/*
#############################################################################
#                                                                           #
# Project: fruits (GUI project)                                             #
# File: mainwindow.hh                                                       #
# Description: instruction.txt-tiedostossa lukee kaikki oleellinen.         #
#                                                                           #
#                                                                           #
#        Ohjelman kirjoittaja                                               #
#            Max Turunen                                                    #
#                                                                           #
#############################################################################
*/


#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <vector>
#include <deque>
#include <random>
#include <ctime>
#include <QTimer>
#include <QLabel>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

const int EMPTY = -1;
const QString FRUITS_SWAPPED = "The selected fruits have been swapped!";
const QString SAME_FRUIT_COORDINATES = "The second fruit cannot have the same coordinates!";
const QString FRUITS_ARE_TOO_FAR_APART = "Fruits must be next to each other vertically or horizontally!";
const QString FIRST_FRUIT_EMPTY = "You cannot choose an empty space as your first fruit!";
const QString MOVED_TO_EMPTY_SPACE = "The fruit has been moved to an empty space!";
const QString CHOOSE_SECOND_FRUIT = "Choose a second fruit!";
const QString CLICK_ON_THE_FRUITS = "Click on the fruits!";
const QString FIRST_INSTRUCTION = "Click on any fruit in order to start the game!";
const QString FRUITS_DROPPED = "The fruits have been dropped!";
const QString GAME_PAUSED = "You paused the timer! Continue by clicking the mouse.";
const int DELAY = 1000;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Removes consecutive fruits, whose coordinates were saved onto the consecutive_fruits_ map structure.
    // In fruits_ vector the removed items' values are changed to nullptr, while the vector's fruits_numbers_ items' values are changed to EMPTY (-1).
    // The fruit images are removed from the scene.
    void remove_consecutive_fruits();

    // Drops the fruits using the coordinates in the drop_these_fruits_ map.
    void drop_fruits();

    // Pauses timer once the user pushes the pause button.
    void pause_timer();

    // Consists of functions that are to do with the timer.
    void on_timeout();

    // Once the with delay button is pressed, the bool variable with_delay_ is set to true.
    void on_with_delay_button_pressed();

    // Once the no delay button is pressed, the bool variable with_delay_ is set to false.
    void on_no_delay_dutton_pressed();

private:
    Ui::MainWindow *ui;

    // Scene for the game grid
    QGraphicsScene* scene_;

    // Margins for the drawing area (the graphicsView object)
    // You can change the values as you wish
    const int TOP_MARGIN = 150;
    const int LEFT_MARGIN = 100;

    // Size of a square containing a fruit
    const int SQUARE_SIDE = 20; // give your own value here
    // Number of vertical cells (places for fruits)
    const int ROWS = 10; // give your own value here
    // Number of horizontal cells (places for fruits)
    const int COLUMNS = 10; // give your own value here

    // Constants describing scene coordinates
    const int BORDER_UP = 0;
    const int BORDER_DOWN = SQUARE_SIDE * ROWS;
    const int BORDER_LEFT = 0;
    const int BORDER_RIGHT = SQUARE_SIDE * COLUMNS;

    // Constants for different fruits and the number of them
    // Add/remove fruits as you wish, or you can remove the whole enum type
    // Actually only the value NUMBER_OF_FRUITS is needed in the
    // template code
    enum Fruit_kind {CHERRY,
                     STRAWBERRY,
                     ORANGE,
                     PEAR,
                     APPLE,
                     BANANAS,
                     TOMATO,
                     GRAPES,
                     EGGPLANT,
                     NUMBER_OF_FRUITS};

    // For randomly selecting fruits for the grid
    std::default_random_engine randomEng_;
    std::uniform_int_distribution<int> distr_;

    // Writes the titles for the grid rows and columns
    void init_titles();

    // Generates fruits for the fruit_numbers_ vector.
    void generate_all_fruits();

    // Vector, that consists of the randomized numbers.
    std::vector<std::vector<int>> fruit_numbers_;

    // Draws a single fruit near the right bottom corner of the grid
    // At the moment, this function is not called in the template,
    // but try to do so, if you want to use real fruits instead of rectangles.
    void draw_fruit_and_push_into_vector();

    // Vector that consists of numbers that are in fruit_numbers_ vector but in the form of fruits (see previous method).
    std::vector<std::vector<QGraphicsPixmapItem*>> fruits_;

    // Insert fruit images to scene from fruits_ vector.
    void add_fruits_to_scene();

    // Sends and publishes text to user_instructions_field, which is in mainwindow.ui.
    // Param1: a QString.
    void send_user_instructions(const QString& text);

    // Saves coordinates from mouse clicks, starts the timer, calls check_chosen_fruit_positions() and swap_fruits(), if they are swappable.
    // Param1: QMouseEvent (clicks)
    void mousePressEvent(QMouseEvent *event);

    // Coordinates for moving the fruits in the game. These coordinates get their values from QMouseEvent from the previous method.
    int first_column_coordinate_ = EMPTY;
    int first_row_coordinate_ = EMPTY;
    int second_column_coordinate_ = EMPTY;
    int second_row_coordinate_ = EMPTY;

    // Timer for the game.
    QTimer* timer;

    // Checks if the fruits chosen by the user are horizontally or vertically next to each other and other things concerning coordinates.
    void check_chosen_fruit_positions();

    // check_chosen_fruit_positions() controls this value. If the given fruit coordinates are horizontally or vertically next to each other fruits are movable.
    bool fruits_are_movable_ = false;

    // If fruits_are_movable, they are moved with the following method. Fruits are moved in three places:
    // fruit_numbers_ vector, fruits_ vector, scene_.
    void swap_fruits();

    //Checks and saves if there are consecutive three fruits in fruit_numbers_ vector. Calls check_and_save_consecutive_vertical_fruits() and check_and_save_consecutive_horizontal_fruits().
    void check_and_save_consecutive_fruits();

    //Looks for consecutive fruits vertically.
    void check_and_save_consecutive_vertical_fruits();

    //Looks for consecutive fruits horizontally
    void check_and_save_consecutive_horizontal_fruits();

    // Map structure to which coordinates of the consecutive fruits are saved.
    std::map<int, std::vector<int>> consecutive_fruits_;

    // Checks if there are any fruits that should be dropped i.e. there are empty spaces under fruits.
    void which_fruits_to_drop();

    // which_fruits_to_drop() inserts true value, if there are fruits that should be dropped.
    bool fruits_should_be_dropped_ = false;

    // Consists of ćoordinates for fruits that should be dropped.
    std::map<int, std::vector<int>> drop_these_fruits_;

    // Used when timer is paused by the user. Once the timer is no longer paused, the start() method is called upon by mousePressEvent.
    bool timer_paused_ = false;

    // Changes values depending on what the user has chosen from the mainwindow.ui Delay options.
    bool with_delay_ = true;

    // Used to divide two fruit coordinates from each other and to coordinate timer (to switch it on).
    int number_of_clicks_ = 0;

    // Sends points onto the screen.
    void count_points();

    // Points, which are added with each fruit removed.
    int points_ = 0;
};
#endif // MAINWINDOW_HH
