#include <iostream>
#include <string>

#include <vector>
#include <algorithm>

#include <fstream>  

struct Point {
    int x, y;

    bool isAdjacent(Point& other) {
        // if they have the same x and the diff between y's is 1
        if (this->x == other.x) {
            if (abs(this->y - other.y) == 1)
                return true;
        }

        if (this->y == other.y) {
            if (abs(this->x - other.x) == 1)
                return true;
        }

        return false;
    }

    // for sorting, based on which has a higher x value
    // if x's are the same, base it on greates y value
    bool isGreaterThan(Point& other) {
        
        if (this->x > other.x) return true;
        if (other.x > this->x) return false;

        if (this->y > other.y) return true;
        if (other.y > this->y) return false;

        // if they are the same, return false
        return false;

    }
    bool operator> (Point& other) const {
        if (this->x > other.x) return true;
        if (other.x > this->x) return false;

        if (this->y > other.y) return true;
        if (other.y > this->y) return false;

        // if they are the same, return false
        return false;
    }
    bool operator< (Point& other) const {
        return !(*this > other);
    }

    bool operator== (const Point& other) const {
        return (this->x == other.x && this->y == other.y);
    }

    bool operator!= (const Point& other) const {
        return !(*this == other);
    }
};

void sortPolyomino(std::vector<Point>& polyomino) {

    // bubble sort
    for (size_t i = 0; i < polyomino.size(); ++i) {
        for (size_t j = i; j < polyomino.size()-1; ++j) {
            // if one first > second, swap them
            if (polyomino.at(j).isGreaterThan(polyomino.at(j + 1))) {
                Point first{ polyomino.at(j) };
                Point second{ polyomino.at(j + 1) };

                polyomino.at(j) = second;
                polyomino.at(j + 1) = first;
            }
                
        }

    }

}

void printPolyomino(const std::vector<Point>& polyomino) {
    for (size_t i = 0; i < polyomino.size(); ++i) {
        std::cout << "(" << polyomino.at(i).x << ", " << polyomino.at(i).y << "), ";
    }
    std::cout << '\n';
}

std::vector<Point> rotatePolyominoCW(const std::vector<Point>& polyomino) {
    std::vector<Point> newPolyomino{ polyomino };
    for (size_t i = 0; i < newPolyomino.size(); ++i) {
        int x = newPolyomino[i].x; int y = newPolyomino[i].y;
        newPolyomino[i].x = y * -1;
        newPolyomino[i].y = x;
    }
    sortPolyomino(newPolyomino);
    return newPolyomino;

}

// sort the polyomino to find the tile at the top left
// then find how far away that one is from 0, 0
// and translate the whole thing by that amount toward 0, 0
void translatePolyominoToTL(std::vector<Point>& polyomino) {
    sortPolyomino(polyomino);

    int furthestLeft = 10000;
    int furthestUp = 10000;

    for (size_t i = 0; i < polyomino.size(); ++i) {
        if (polyomino.at(i).x < furthestLeft) furthestLeft = polyomino.at(i).x;
        if (polyomino.at(i).y < furthestUp) furthestUp = polyomino.at(i).y;
    }

    /*int xTrans = 0 - polyomino.at(0).x;
    int yTrans = 0 - polyomino.at(0).y;*/
    int xTrans = 0 - furthestLeft;
    int yTrans = 0 - furthestUp;

    for (size_t i = 0; i < polyomino.size(); ++i) {
        polyomino.at(i).x += xTrans;
        polyomino.at(i).y += yTrans;
    }


}

void drawPolyomino(const std::vector<Point>& mino) {
    std::vector<Point> temp{ mino };

    translatePolyominoToTL(temp);

    std::vector<std::vector<int>> poly(temp.size(), std::vector<int>(temp.size(), 0));

    for (size_t i = 0; i < temp.size(); ++i) {
        poly[temp[i].x][temp[i].y] = 1;
    }

    for (size_t i = 0; i < poly.size(); ++i) {
        for (size_t j = 0; j < poly.at(i).size(); ++j) {
            if (poly[i][j] == 0) std::cout << ".";
            else std::cout << "#";
        }
        std::cout << '\n';
    }
    std::cout << '\n';

}

// Generate all rotations of a polyomino
// and translate them to the top left
std::vector<std::vector<Point>> generateChecks(std::vector<Point>& mino) {
    std::vector<std::vector<Point>> checks; checks.reserve(4);
    checks.push_back(mino);                           // no rot
    checks.push_back(rotatePolyominoCW(checks.at(0)));// rot once
    checks.push_back(rotatePolyominoCW(checks.at(1)));// rot twice
    checks.push_back(rotatePolyominoCW(checks.at(2)));// rot thrice
    for (size_t i = 0; i < checks.size(); ++i) { // for each check
        translatePolyominoToTL(checks.at(i));  // translate it to the top left
        sortPolyomino(checks.at(i));
    }
    return checks;
}

bool polyominoesAreEqual(const std::vector<Point>& first, const std::vector<Point>& second) {
    if (first.size() != second.size()) return false;
    for (size_t i = 0; i < first.size(); ++i) {
        if (first.at(i) != second.at(i)) return false;
    }
    return true;
}

// compares all 4 rotaions of the first to all 4 rotations of the second
// pass in all 4 rotaions of each polyomino
bool polyominoesAreEqualUsingChecks(std::vector<std::vector<Point>>& checks, std::vector<std::vector<Point>>& otherChecks) {
    for (size_t k = 0; k < checks.size(); ++k)
        for (size_t l = 0; l < otherChecks.size(); ++l)
            if(polyominoesAreEqual(checks.at(k), otherChecks.at(l))) return true;
    return false;

}

// recursive
// each iteration, for each tile of the polyomino it will add another tile onto it for each possible spot
// i.e iteration 1 will start with just #
// 2: ##
//
// 3: ##    and   ###
//    #   
// pass in number of tiles per polyomino for MAX_ITERATIONS, and 0 for n
std::vector<std::vector<Point>> generatePolyominoes(std::vector<std::vector<Point>>& previous, int MAX_ITERATIONS, int n = 0){
    if (n == MAX_ITERATIONS) return previous;

    // if n == 0, then then previous should start with a domino since every polyomino >3 is derived from a domino
    //if (n == 0)  previous.push_back(std::vector<Point>{Point{ 0, 0 }, Point{ 0, 1 }});
    if (n <= 1) {
        n = 1;
        previous.push_back(std::vector<Point>{Point{ 0, 0 }});

        if (MAX_ITERATIONS <= 1) return previous;
    }
    
    std::vector<std::vector<Point>> newPolyominoes;
    newPolyominoes.reserve(n * n); // n^2 should be sufficient



    // Generate all possible branching polyominoes (this will make duplicates)
    for (size_t i = 0; i < previous.size(); ++i) {
        std::vector<Point> mino{ previous.at(i) };

        // for each tile of this polyomino, add a tile to every spot around it
        for (size_t j = 0; j < previous.at(i).size(); ++j) {
            Point center{ previous.at(i).at(j) }; // the tile being added onto

            Point west{ center.x - 1, center.y };
            Point east{ center.x + 1, center.y };
            Point north{ center.x, center.y - 1 };
            Point south{ center.x, center.y + 1};

            // if any true, then that means that the cooresponding tile is already occupied
            bool hasWest{ false }; bool hasEast{ false }; bool hasNorth{ false }; bool hasSouth{ false };
            
            // check to see if this polyomino already contains west, east, north, or south
            for (size_t k = 0; k < previous.at(i).size(); ++k) {
                if (mino.at(k) == west) hasWest = true;
                if (mino.at(k) == east) hasEast = true;
                if (mino.at(k) == north) hasNorth = true;
                if (mino.at(k) == south) hasSouth = true;
            }
            if (!hasWest) { // if the tile isn't overlapping any tiles already there
                std::vector<Point> newMino{ mino };
                newMino.push_back(west);           // create a new polyomino with it 
                newPolyominoes.push_back(newMino); // and add it to the new polyominoes list
            }
            // do the same for e, n, and s
            if (!hasSouth) {
                std::vector<Point> newMino{ mino };
                newMino.push_back(south);         
                newPolyominoes.push_back(newMino); 
            }
            if (!hasEast) { 
                std::vector<Point> newMino{ mino };
                newMino.push_back(east);          
                newPolyominoes.push_back(newMino);
            }
            if (!hasNorth) {
                std::vector<Point> newMino{ mino };
                newMino.push_back(north);           
                newPolyominoes.push_back(newMino);
            }


        }
    }

    for (size_t i = 0; i < newPolyominoes.size(); ++i) sortPolyomino(newPolyominoes.at(i));

    // Now that we have all possible branching polyominoes, remove duplicates
    for (size_t i = 0; i < newPolyominoes.size(); ++i) { // for each polyomino
        bool isDuplicate{ false };

        std::vector<std::vector<Point>> checks = generateChecks(newPolyominoes.at(i));

        for (size_t j = 0; j < newPolyominoes.size(); ++j) { // for each other polyomino
            if (i == j) continue;

            std::vector<std::vector<Point>> otherChecks = generateChecks(newPolyominoes.at(j));

            if (polyominoesAreEqualUsingChecks(checks, otherChecks)) {
                isDuplicate = true;
                break;
            }
            
        }
        if (isDuplicate) {
            newPolyominoes.erase(newPolyominoes.begin() + i);
            --i;
        }
    }
    
    std::cout << "Completed for n = " << n << '\n'; // for showing progress
    return generatePolyominoes(newPolyominoes, MAX_ITERATIONS, n + 1);
}


void outputPolyominoesToFile(std::string fileName, std::vector<std::vector<Point>>& minos) {
    std::ofstream file(fileName);
    for (size_t i = 0; i < minos.size(); ++i) {
        for (size_t j = 0; j < minos.at(i).size(); ++j)
            file << "(" << minos.at(i).at(j).x << ", " << minos.at(i).at(j).y << "), ";
        file << "\n";
    }
    file.close();
}

int main() {
    // i realize that this calls generatePolyominoes() stupidly, but whatever
    for (int i = 1; i <= 9; ++i) {
        int n = i;
        std::cout << n << "A\n";

        std::vector<std::vector<Point>> a{};
        std::vector<std::vector<Point>> minos{ generatePolyominoes(a, n) };

        for (auto& m : minos) translatePolyominoToTL(m);

        outputPolyominoesToFile(std::string("Polyominoes/Polyominoes Size ") + std::to_string(n) + std::string(".txt"), minos);
        std::cout << "outputted for n = " << n << '\n';
    }


    //int n = 7;

    //std::vector<std::vector<Point>> a{};
    //std::vector<std::vector<Point>> minos{ generatePolyominoes(a, n) };

    //for (size_t i = 0; i < minos.size(); ++i) {
    //    translatePolyominoToTL(minos.at(i));
    //    printPolyomino(minos.at(i));
    //}

    //for (size_t i = 0; i < minos.size(); ++i)
    //    drawPolyomino(minos.at(i));

    //std::cout << '\n' << "n = " << n << '\n';
    //std::cout << "Number of polyominoes: " << minos.size() << '\n';





    return 0;

}

