/*
#############################################################################
#                                                                           #
# Project: Fruits (GUI project)                                             #
# File: mainwindow.cpp                                                      #
# Description: instruction.txt-tiedostossa lukee kaikki oleellinen.         #
#                                                                           #
#                                                                           #
#        Ohjelman kirjoittaja                                               #
#        Max Turunen                                                        #
#############################################################################
*/

#include "mainwindow.hh"
#include "ui_mainwindow.h"
#include <QKeyEvent>
#include <QDebug>
#include <QPixmap>
#include <vector>
#include <map>
#include <iostream>
#include <QMouseEvent>
#include <QDebug>
#include <cmath>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // We need a graphics scene in which to draw rectangles
    scene_ = new QGraphicsScene(this);

    // The width of the graphicsView is BORDER_RIGHT added by 2,
    // since the borders take one pixel on each side
    // (1 on the left, and 1 on the right).
    // Similarly, the height of the graphicsView is BORDER_DOWN added by 2.
    ui->graphicsView->setGeometry(LEFT_MARGIN, TOP_MARGIN,
                                  BORDER_RIGHT + 2, BORDER_DOWN + 2);
    ui->graphicsView->setScene(scene_);

    // The width of the scene_ is BORDER_RIGHT decreased by 1 and
    // the height of it is BORDER_DOWN decreased by 1, because
    // each square of a fruit is considered to be inside the sceneRect,
    // if its upper left corner is inside the sceneRect.
    scene_->setSceneRect(0, 0, BORDER_RIGHT - 1, BORDER_DOWN - 1);

    int seed = time(0); // You can change seed value for testing purposes
    randomEng_.seed(seed);
    distr_ = std::uniform_int_distribution<int>(0, NUMBER_OF_FRUITS - 1);
    distr_(randomEng_); // Wiping out the first random number (which is almost always 0)

    // Sets the background color for the lcd screens.
    QPalette palette;
    palette.setColor(QPalette::Background, Qt::black);
    ui->points_lcd->setPalette(palette);
    ui->points_lcd->setAutoFillBackground(true);

    // Initiate timer...
    timer = new QTimer();
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, &MainWindow::on_timeout);
    connect(ui->pause_button, &QPushButton::clicked, this, &MainWindow::pause_timer);

    init_titles();

    // Generate fruits and show them in the scene...
    generate_all_fruits();
    draw_fruit_and_push_into_vector();
    add_fruits_to_scene();
    send_user_instructions(FIRST_INSTRUCTION);
}


MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
    for (std::vector<QGraphicsPixmapItem*> vector: fruits_)
    {
        for (auto fruit_pointer : vector)
        {
            delete fruit_pointer;
        }
    }
}


void MainWindow::init_titles()
{
    // Displaying column titles, starting from A
    for(int i = 0, letter = 'A'; i < COLUMNS; ++i, ++letter)
    {
        int shift = 5;
        QString letter_string = "";
        letter_string.append(letter);
        QLabel* label = new QLabel(letter_string, this);
        label->setGeometry(LEFT_MARGIN + shift + i * SQUARE_SIDE,
                           TOP_MARGIN - SQUARE_SIDE,
                           SQUARE_SIDE, SQUARE_SIDE);
    }
    // Displaying row titles, starting from A
    for(int i = 0, letter = 'A'; i < ROWS; ++i, ++letter)
    {
        QString letter_string = "";
        letter_string.append(letter);
        QLabel* label = new QLabel(letter_string, this);
        label->setGeometry(LEFT_MARGIN - SQUARE_SIDE,
                           TOP_MARGIN + i * SQUARE_SIDE,
                           SQUARE_SIDE, SQUARE_SIDE);
    }
}


void MainWindow::generate_all_fruits()
{
    bool third_same_column_number = false;
    bool third_same_row_number = false;
    bool previous_two_column_numbers_are_the_same = false;
    bool previous_two_row_numbers_are_the_same = false;
    std::vector<std::vector<int>> tmp_container_vector;
    for (int column = 0; column < COLUMNS; ++column)
    {
        std::vector<int> tmp_inner_vector;
        for (int row = 0; row < ROWS; ++row)
        {
            int tmp_random_number = distr_(randomEng_);
            // Checks if previous columns have same numbers at the same row index.
            if (tmp_container_vector.size() >= 2)
            {
                // Since we want to check if previous columns have the same number as we are going to put into our row now, we don't need to subtract row...
                if (tmp_container_vector.at(column-1).at(row) == tmp_container_vector.at(column-2).at(row))
                {
                    previous_two_column_numbers_are_the_same = true;
                }
                // If previous two inner vectors have the same fruit number and the generated number is also the same...
                if (previous_two_column_numbers_are_the_same && tmp_container_vector.at(column-1).at(row) ==  tmp_random_number)
                {
                    third_same_column_number = true;
                }
            }
            // Once two row numbers are generated, check if the third row number is the same.
            if (tmp_inner_vector.size() >= 2 )
            {
                if (tmp_inner_vector.at(row-1) == tmp_inner_vector.at(row-2))
                {
                    previous_two_row_numbers_are_the_same = true;
                }
                // If two first numbers are the same as the new generated number change tmp_random_number to another one.
                if (previous_two_row_numbers_are_the_same && tmp_inner_vector.at(row-1) == tmp_random_number)
                {
                    third_same_row_number = true;
                }
            }
            // Generate new random_number according to which number is acceptable.
            // To make sure, that new number does not become a third same number of the sequency, we chech in inner if-clauses that this doesn't happen.
            if (third_same_row_number && !third_same_column_number)
            {
                while (tmp_random_number == tmp_inner_vector.at(row-1))
                {
                    tmp_random_number = distr_(randomEng_);
                }
            } else if (!third_same_row_number && third_same_column_number)
            {
                while (tmp_random_number == tmp_container_vector.at(column-1).at(row))
                {
                    tmp_random_number = distr_(randomEng_);
                }
            } else if (third_same_row_number && third_same_column_number)
            {
                while (tmp_random_number == tmp_inner_vector.at(row-1))
                {
                    tmp_random_number = distr_(randomEng_);
                }
            }
            tmp_inner_vector.push_back(tmp_random_number);
            third_same_row_number = false;
            third_same_column_number = false;
            previous_two_row_numbers_are_the_same = false;
            previous_two_column_numbers_are_the_same = false;
        }
        tmp_container_vector.push_back(tmp_inner_vector);
    }
    fruit_numbers_ = tmp_container_vector;
}


void MainWindow::draw_fruit_and_push_into_vector()
{
    // Vector of fruits
    const std::vector<std::string>
            fruits = {"cherries", "strawberry", "orange", "pear", "apple",
                      "bananas", "tomato", "grapes", "eggplant"};

    // Defining where the images can be found and what kind of images they are
    const std::string PREFIX(":/");
    const std::string SUFFIX(".png");

    // Converting image (png) to a pixmap
    for (int column = 0; column < COLUMNS; ++column)
    {
        std::vector<QGraphicsPixmapItem*> tmp_inner_vector;
        for (int row = 0; row < ROWS; ++row)
        {
            std::string filename = PREFIX + fruits.at(fruit_numbers_.at(column).at(row)) + SUFFIX;
            QPixmap square_pixmap(QString::fromStdString(filename));
            QGraphicsPixmapItem* fruit_img = new QGraphicsPixmapItem;
            fruit_img->setPixmap(square_pixmap.scaled(SQUARE_SIDE, SQUARE_SIDE));
            tmp_inner_vector.push_back(fruit_img);
        }
        fruits_.push_back(tmp_inner_vector);
    }
}


void MainWindow::add_fruits_to_scene()
{
    for (int column = 0; column < COLUMNS; ++column)
    {
        for (int row = 0; row < ROWS; ++row)
        {
            fruits_.at(column).at(row)->setPos(column * SQUARE_SIDE, row * SQUARE_SIDE);
            scene_->addItem(fruits_.at(column).at(row));
        }
    }
}


void MainWindow::send_user_instructions(const QString& text)
{
    ui->user_instructions_field->setText(text);
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    int column_coordinate = event->x() ;
    int row_coordinate = event->y() ;
    column_coordinate = (column_coordinate - LEFT_MARGIN)/ SQUARE_SIDE;
    row_coordinate = (row_coordinate - TOP_MARGIN)/ SQUARE_SIDE;

    //The first click starts the timer.
    if (number_of_clicks_ == 0 || timer_paused_)
    {
        timer->start();
    }
    ++number_of_clicks_;
    if (number_of_clicks_ % 2 != 0)
    {
        first_column_coordinate_ = column_coordinate;
        first_row_coordinate_ = row_coordinate;
    } else {
        second_column_coordinate_ = column_coordinate;
        second_row_coordinate_ = row_coordinate;
    }
    check_chosen_fruit_positions();
    if (fruits_are_movable_)
    {
        swap_fruits();
    }
}



void MainWindow::check_chosen_fruit_positions()
{
    if (first_column_coordinate_ >= COLUMNS || first_column_coordinate_ < 0 ||
            first_row_coordinate_ >= ROWS || first_row_coordinate_ < 0 ||second_column_coordinate_ >= COLUMNS ||
            second_column_coordinate_ < 0 ||second_row_coordinate_ >= ROWS ||second_row_coordinate_ < 0)
    {
        send_user_instructions(CLICK_ON_THE_FRUITS);
        fruits_are_movable_ = false;
    } else if (number_of_clicks_ % 2 == 0)
    {
        if (abs (first_column_coordinate_ - second_column_coordinate_) > 1)
        {
            send_user_instructions(FRUITS_ARE_TOO_FAR_APART);
            fruits_are_movable_ = false;
        } else if (abs (first_row_coordinate_ - second_row_coordinate_) > 1)
        {
            send_user_instructions(FRUITS_ARE_TOO_FAR_APART);
            fruits_are_movable_ = false;
        }else if (first_column_coordinate_ == second_column_coordinate_)
        {
            if (first_row_coordinate_ == second_row_coordinate_)
            {
                send_user_instructions(SAME_FRUIT_COORDINATES);
                fruits_are_movable_ = false;
            } else if (abs (first_row_coordinate_ - second_row_coordinate_) == 1)
            {
                send_user_instructions(FRUITS_SWAPPED);
                fruits_are_movable_ = true;
            }
        } else if (first_row_coordinate_ == second_row_coordinate_)
        {
            if (first_column_coordinate_ == second_column_coordinate_)
            {
                send_user_instructions(SAME_FRUIT_COORDINATES);
                fruits_are_movable_ = false;
            }else if (abs (first_column_coordinate_ - second_column_coordinate_) == 1)
            {
                send_user_instructions(FRUITS_SWAPPED);
                fruits_are_movable_ = true;
            }
        } else if (abs (first_row_coordinate_ - second_row_coordinate_) == 1)
        {
            if (abs (first_column_coordinate_ - second_column_coordinate_) == 1)
            {
                send_user_instructions(FRUITS_ARE_TOO_FAR_APART);
                fruits_are_movable_ = false;
            } else if (first_column_coordinate_ == second_column_coordinate_)
            {
                send_user_instructions(FRUITS_SWAPPED);
                fruits_are_movable_ = true;
            }
        } else if (abs (first_column_coordinate_ - second_column_coordinate_) == 1)
        {
            if (abs (first_row_coordinate_ - second_row_coordinate_) == 1)
            {
                send_user_instructions(FRUITS_ARE_TOO_FAR_APART);
                fruits_are_movable_ = false;
            } else if (first_row_coordinate_ == second_row_coordinate_)
            {
                send_user_instructions(FRUITS_SWAPPED);
                fruits_are_movable_ = true;
            }
        }
    } else if (number_of_clicks_ % 2 != 0)
    {
        int first_fruit_number = fruit_numbers_.at(first_column_coordinate_).at(first_row_coordinate_);
        if (first_fruit_number == EMPTY)
        {
            send_user_instructions(FIRST_FRUIT_EMPTY);
            fruits_are_movable_ = false;
            --number_of_clicks_;
        } else {
            send_user_instructions(CHOOSE_SECOND_FRUIT);
            fruits_are_movable_ = false;
        }
    }
}


void MainWindow::check_and_save_consecutive_fruits()
{
    check_and_save_consecutive_vertical_fruits();
    check_and_save_consecutive_horizontal_fruits();
    if (with_delay_)
    {
        QTimer::singleShot(DELAY, this, SLOT(remove_consecutive_fruits()));
    }else
    {
        remove_consecutive_fruits();
    }
}


void MainWindow::check_and_save_consecutive_vertical_fruits()
{
    // Looking for consecutive fructs vertically...
    for (int column = 0; column < COLUMNS; ++column)
    {
        int sum_of_consecutive_numbers = -1;
        // Initialize tmp_number as first item in the vector. That's why sum_of_consecutive_numbers is initialized as -1.
        int tmp_number = fruit_numbers_.at(column).at(0);
        for (int row = 0; row < ROWS; ++row)
        {
            int new_number = fruit_numbers_.at(column).at(row);
            if ((new_number != EMPTY) && (new_number == tmp_number))
            {
                // Since there will be two consecutive numbers in the first instance...
                if (sum_of_consecutive_numbers == 0)
                {
                    sum_of_consecutive_numbers += 2;
                } else {
                    ++sum_of_consecutive_numbers;
                }
            }
            // Once the consecutive_numbers have ended or once the vector has reached its end, we insert them into a map structure.
            if ((sum_of_consecutive_numbers >= 3 && new_number != tmp_number))
            {
                for (int i = 1; i <= sum_of_consecutive_numbers; ++i)
                {
                    // If column index already exists...
                    if (consecutive_fruits_.find(column) != consecutive_fruits_.end())
                    {
                        consecutive_fruits_.at(column).push_back(row - i);
                    } else {
                        std::vector <int> tmp_vector;
                        tmp_vector.push_back(row - i);
                        std::pair <int, std::vector<int>> tmp_pair (column, tmp_vector);
                        consecutive_fruits_.insert(tmp_pair);
                    }
                }
                sum_of_consecutive_numbers = 0;
            }else if (sum_of_consecutive_numbers >= 3 && row == (ROWS - 1))
            {
                for (int i = 0; i < sum_of_consecutive_numbers; ++i)
                {
                    // If column index already exists...
                    if (consecutive_fruits_.find(column) != consecutive_fruits_.end())
                    {
                        consecutive_fruits_.at(column).push_back(row - i);
                    } else {
                        std::vector <int> tmp_vector;
                        tmp_vector.push_back(row - i);
                        std::pair <int, std::vector<int>> tmp_pair (column, tmp_vector);
                        consecutive_fruits_.insert(tmp_pair);
                    }
                }
                sum_of_consecutive_numbers = 0;
            } else if (sum_of_consecutive_numbers < 3 && new_number != tmp_number) {
                sum_of_consecutive_numbers = 0;
            }
            //Set tmp_number as new number for a new round of iteration.
            tmp_number = new_number;
        }
    }
}


void MainWindow::check_and_save_consecutive_horizontal_fruits()
{
    // Looking for horizontal consecutive fruits.
    for (int row = 0; row < ROWS; ++row)
    {
        int sum_of_consecutive_numbers = -1;
        int tmp_number = fruit_numbers_.at(row).at(0);
        for (int column = 0; column < COLUMNS; ++column)
        {
            int new_number = fruit_numbers_.at(column).at(row);

            if (((new_number != EMPTY) && (new_number == tmp_number)))
            {
                if (sum_of_consecutive_numbers == 0)
                {
                    sum_of_consecutive_numbers += 2;
                } else {
                    ++sum_of_consecutive_numbers;
                }
            }
            // Once the consecutive_numbers have ended they are inserted into a map, which keeps track of the fruits that are consecutive.
            if (sum_of_consecutive_numbers >= 3 && new_number != tmp_number)
            {
                for (int i = 1; i <= sum_of_consecutive_numbers; ++i)
                {
                    // If column index already exists...
                    if (consecutive_fruits_.find(column - i) != consecutive_fruits_.end())
                    {
                        consecutive_fruits_.at(column - i).push_back(row);
                    } else {
                        std::vector <int> tmp_vector;
                        tmp_vector.push_back(row);
                        std::pair <int, std::vector<int>> tmp_pair (column  - i, tmp_vector);
                        consecutive_fruits_.insert(tmp_pair);
                    }
                }
                sum_of_consecutive_numbers = 0;
            } else if (sum_of_consecutive_numbers >= 3 && column == (COLUMNS - 1))
            {
                for (int i = 0; i < sum_of_consecutive_numbers; ++i)
                {
                    // If column index already exists...
                    if (consecutive_fruits_.find(column - i) != consecutive_fruits_.end())
                    {
                        consecutive_fruits_.at(column - i).push_back(row);
                    } else {
                        std::vector <int> tmp_vector;
                        tmp_vector.push_back(row);
                        std::pair <int, std::vector<int>> tmp_pair (column  - i, tmp_vector);
                        consecutive_fruits_.insert(tmp_pair);
                    }
                }
                sum_of_consecutive_numbers = 0;
            } else if (sum_of_consecutive_numbers < 3 && new_number != tmp_number) {
                sum_of_consecutive_numbers = 0;
            }
            tmp_number = new_number;
        }
    }
}


// Removes fruits or marks them as EMPTY. Once the map structure is employed, it is cleared in order to not delete fruits that will be placed in empty spaces.
void MainWindow::remove_consecutive_fruits()
{
    int earned_points = 0;
    if (!consecutive_fruits_.empty())
    {
        for (auto pair: consecutive_fruits_)
        {
            int column_index = pair.first;
            for (auto row_index : pair.second)
            {
                fruit_numbers_.at(column_index).at(row_index) = EMPTY;
                if (fruits_.at(column_index).at(row_index) != nullptr)
                {
                    fruits_.at(column_index).at(row_index)->hide();
                    fruits_.at(column_index).at(row_index) = nullptr;
                    ++earned_points;
                }
            }
        }
        // Points are doubled if the user aligned more than three fruits at one time.
        if (earned_points > 3)
        {
            earned_points *= 2;
        }
        points_ += earned_points;
        count_points();
    }
    consecutive_fruits_.clear();
    which_fruits_to_drop();
    if (with_delay_)
    {
        QTimer::singleShot(DELAY, this, SLOT(drop_fruits()));
    }else
    {
        drop_fruits();
    }
}


void MainWindow::swap_fruits()
{
    int first_fruit_number = fruit_numbers_.at(first_column_coordinate_).at(first_row_coordinate_);
    int second_fruit_number = fruit_numbers_.at(second_column_coordinate_).at(second_row_coordinate_);
    if (second_fruit_number == EMPTY)
    {
        // Moving the fruit image on the screen...
        fruits_.at(first_column_coordinate_).at(first_row_coordinate_)->setPos(second_column_coordinate_ * SQUARE_SIDE, second_row_coordinate_ * SQUARE_SIDE);
        // Swapping fruit_numbers...
        fruit_numbers_.at(second_column_coordinate_).at(second_row_coordinate_) = first_fruit_number;
        fruit_numbers_.at(first_column_coordinate_).at(first_row_coordinate_) = second_fruit_number;
        //Swapping imagepointers...
        QGraphicsPixmapItem* first_fruit = fruits_.at(first_column_coordinate_).at(first_row_coordinate_);
        QGraphicsPixmapItem* second_fruit = fruits_.at(second_column_coordinate_).at(second_row_coordinate_);
        fruits_.at(second_column_coordinate_).at(second_row_coordinate_) = first_fruit;
        fruits_.at(first_column_coordinate_).at(first_row_coordinate_) = second_fruit;
        send_user_instructions(MOVED_TO_EMPTY_SPACE);
    } else {
        fruit_numbers_.at(second_column_coordinate_).at(second_row_coordinate_) = first_fruit_number;
        fruit_numbers_.at(first_column_coordinate_).at(first_row_coordinate_) = second_fruit_number;
        //Swapping images...
        QGraphicsPixmapItem* first_fruit = fruits_.at(first_column_coordinate_).at(first_row_coordinate_);
        QGraphicsPixmapItem* second_fruit = fruits_.at(second_column_coordinate_).at(second_row_coordinate_);
        fruits_.at(second_column_coordinate_).at(second_row_coordinate_) = first_fruit;
        fruits_.at(first_column_coordinate_).at(first_row_coordinate_) = second_fruit;
        // Moving images on the screen... Since images have been swapped, we need to update their placement only.
        fruits_.at(first_column_coordinate_).at(first_row_coordinate_)->setPos(first_column_coordinate_ * SQUARE_SIDE, first_row_coordinate_ * SQUARE_SIDE);
        fruits_.at(second_column_coordinate_).at(second_row_coordinate_)->setPos(second_column_coordinate_ * SQUARE_SIDE, second_row_coordinate_ * SQUARE_SIDE);
    }
    check_and_save_consecutive_fruits();
    fruits_are_movable_ = false;
}


void MainWindow::which_fruits_to_drop()
{
    int empty_spaces_in_a_column = 0;
    for (int column = 0; column < COLUMNS; ++column)
    {
        for (int row = ROWS - 1; row >= 0; --row)
        {
            if (fruit_numbers_.at(column).at(row) == EMPTY)
            {
                empty_spaces_in_a_column++;
                // Check if item has preceding items.
                if (row > 0)
                {
                    for (int second_row = row-1; second_row >= 0; --second_row)
                    {
                        // Check if there are previous items that are not empty.
                        // If such items occur, it means that these items must be dropped.
                        // If there are only empty spaces, there's nothing to drop.
                        if (fruit_numbers_.at(column).at(second_row) != EMPTY)
                        {
                            fruits_should_be_dropped_ = true;
                            if (drop_these_fruits_.find(column) != drop_these_fruits_.end())
                            {
                                drop_these_fruits_.at(column).push_back(second_row);
                            } else {
                                std::vector <int> tmp_vector;
                                tmp_vector.push_back(second_row);
                                std::pair <int, std::vector<int>> tmp_pair (column, tmp_vector);
                                drop_these_fruits_.insert(tmp_pair);
                            }
                        }
                    }

                }
                // Since we found an empty row that is as close to last index as possible, there is no need to iterate any further.
                break;
            }
        }
    }
}


void MainWindow::drop_fruits()
{
    if (fruits_should_be_dropped_)
    {
        bool is_ready_to_drop = false;
        for (int column = 0; column < COLUMNS; ++ column)
        {
            int drop_here_index = 0;
            is_ready_to_drop = false;
            // Since the fruits are dropped to the last empty item, iteration takes place backwards.
            for (int row = ROWS-1; row >= 0; --row)
            {
                // Once the first instance of an empty item is found (counting backwards), its index is saved and the loop is broken from.
                if (fruit_numbers_.at(column).at(row) == EMPTY)
                {
                    is_ready_to_drop = true;
                    drop_here_index = row;
                    break;
                }
            }
            if (is_ready_to_drop)
            {
                // If there are droppable fruits in the map, they are dropped.
                if (drop_these_fruits_.find(column) != drop_these_fruits_.end())
                {
                    // Just to be sure the key has also a value...
                    if (!drop_these_fruits_.at(column).empty())
                    {
                        for (int fruit_index : drop_these_fruits_.at(column))
                        {
                            int fruit_number_to_be_dropped = fruit_numbers_.at(column).at(fruit_index);
                            fruit_numbers_.at(column).at(drop_here_index) = fruit_number_to_be_dropped;
                            QGraphicsPixmapItem* fruit_picture_to_be_dropped = fruits_.at(column).at(fruit_index);
                            // Moving the fruit image.
                            fruits_.at(column).at(fruit_index)->setPos(column * SQUARE_SIDE, drop_here_index * SQUARE_SIDE);
                            // Setting new fruit image in the empty space.
                            fruits_.at(column).at(drop_here_index) = fruit_picture_to_be_dropped;

                            if (drop_here_index > 0)
                            {
                                --drop_here_index;
                            }
                            send_user_instructions(FRUITS_DROPPED);
                        }
                        // The drop_here_index now indicates the closest item to dropped fruits, the items that precede them should be empty.
                        for (; drop_here_index >= 0; --drop_here_index)
                        {
                            fruit_numbers_.at(column).at(drop_here_index) = EMPTY;
                            if (fruits_.at(column).at(drop_here_index) != nullptr)
                            {
                                fruits_.at(column).at(drop_here_index) = nullptr;
                            }
                        }
                    }
                }
            }
        }
        fruits_should_be_dropped_ = false;
        drop_these_fruits_.clear();
        check_and_save_consecutive_fruits();
    }
}


void MainWindow::on_timeout()
{
    int current_secs = ui->seconds_lcd->intValue();
    int current_mins = ui->minutes_lcd->intValue();

    if (current_secs == 59)
    {
        ui->minutes_lcd->display(current_mins + 1);
        ui->seconds_lcd->display(0);
    } else {
        ui->seconds_lcd->display(current_secs+ 1);
    }
}


void MainWindow::pause_timer()
{
    timer->stop();
    timer_paused_ = true;
    send_user_instructions(GAME_PAUSED);
}


void MainWindow::count_points()
{
    ui->points_lcd->display(points_);
}


void MainWindow::on_with_delay_button_pressed()
{
    with_delay_ = true;
}

void MainWindow::on_no_delay_dutton_pressed()
{
    with_delay_ = false;
}
