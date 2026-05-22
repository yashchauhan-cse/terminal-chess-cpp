#include<iostream>
#include<string>
#include<vector>
#include<utility>
#include<cstdlib>
#include <map>

using namespace std;

#define RESET       "\033[0m"
#define NEON_CYAN   "\033[1;96m"
#define NEON_YELLOW "\033[1;93m"
#define DIM_GRAY    "\033[90m"
#define BRIGHT_RED  "\033[1;91m"
#define NEON_GREEN  "\033[1;92m"

class Coord {
public:
    int x, y;
    Coord(int x, int y) : x(x), y(y) {}
    bool isValid() const {
        return x >= 0 && x < 8 && y >= 0 && y < 8;
    }
};

vector<char> blackCaptured, whiteCaptured;
int halfMoveClock = 0;
map<string, int> positionHistory;

class LastMove {
public:
    Coord from;
    Coord to;
    string piece;
    string colour;
    LastMove() : from(Coord(-1,-1)), to(Coord(-1,-1)), piece(""), colour("") {}
    LastMove(Coord from, Coord to, string piece, string colour) : from(from), to(to), piece(piece), colour(colour) {}
};

LastMove lastMove;

string to_lower(string str);
string boardToString();

Coord findKing(string colour);

int materialAdvantage();

void fillBoard();
void cleanBoard();
void clearScreen();
void displayBoard();
void promotion(string colour, Coord dst);

bool sourceMoveValidate(Coord src);
bool destMoveValidate(Coord dst, Coord src);
bool inBounds(Coord c);
bool takeMove(bool whiteTurn);
bool isCheck(string colour);
bool isCheckmate(string colour);
bool isStalemate(string colour);

class Piece;
Piece* board[8][8] = {nullptr};

class Piece {
public:
    string colour;
    string name;
    char symbol;
    Coord pos;
    vector<Coord> moves;
    Piece(string c, string name, char sym, Coord p) : colour(c), name(name), symbol(sym), pos(p) {}
    virtual bool canMove(Coord src, Coord dst) = 0;
    virtual void findMoves(Coord src) = 0;
    virtual int getValue() const = 0;
    virtual ~Piece() {}
};

class Knight : public Piece {
public:
    int getValue() const { return 3; }
    Knight(string c, string name, char sym, Coord pos) : Piece(c, name, sym, pos) {}
    bool canMove(Coord src, Coord dst) override {
        int X = abs(dst.x - src.x);
        int Y = abs(dst.y - src.y);
        if ((X == 1 && Y == 2) || (X == 2 && Y == 1)) {
            return true;
        } else {
            return false;
        }
    }
    void findMoves(Coord src) override{
        moves.clear();
        Coord p(src.x, src.y);
        p.x = src.x + 1; p.y = src.y + 2;
        if (p.isValid()) if (board[p.y][p.x] == nullptr || board[p.y][p.x]->colour != this->colour) moves.push_back(p);
        p.x = src.x - 1; p.y = src.y + 2;
        if (p.isValid()) if (board[p.y][p.x] == nullptr || board[p.y][p.x]->colour != this->colour) moves.push_back(p);
        p.x = src.x + 1; p.y = src.y - 2;
        if (p.isValid()) if (board[p.y][p.x] == nullptr || board[p.y][p.x]->colour != this->colour) moves.push_back(p);
        p.x = src.x - 1; p.y = src.y - 2;
        if (p.isValid()) if (board[p.y][p.x] == nullptr || board[p.y][p.x]->colour != this->colour) moves.push_back(p);
        p.x = src.x + 2; p.y = src.y + 1;
        if (p.isValid()) if (board[p.y][p.x] == nullptr || board[p.y][p.x]->colour != this->colour) moves.push_back(p);
        p.x = src.x - 2; p.y = src.y + 1;
        if (p.isValid()) if (board[p.y][p.x] == nullptr || board[p.y][p.x]->colour != this->colour) moves.push_back(p);
        p.x = src.x - 2; p.y = src.y - 1;
        if (p.isValid()) if (board[p.y][p.x] == nullptr || board[p.y][p.x]->colour != this->colour) moves.push_back(p);
        p.x = src.x + 2; p.y = src.y - 1;
        if (p.isValid()) if (board[p.y][p.x] == nullptr || board[p.y][p.x]->colour != this->colour) moves.push_back(p);
    }   
};

class Bishop : public Piece {
public:
    int getValue() const { return 3; }
    Bishop(string c, string name, char sym, Coord pos) : Piece(c, name, sym, pos) {}
    bool canMove(Coord src, Coord dst) override {
        int X = abs(dst.x - src.x);
        int Y = abs(dst.y - src.y);
        
        if (X != Y || X == 0 || Y == 0) return false;
        
        int stepX = (dst.x > src.x) ? 1 : -1;
        int stepY = (dst.y > src.y) ? 1 : -1;
        int currentX = src.x + stepX;
        int currentY = src.y + stepY;
        for (int i = 0; i < X - 1; i++) {
            if (board[currentY][currentX] != nullptr) return false;
            currentX += stepX;
            currentY += stepY;
        }
        return true;
    }
    void findMoves(Coord src) override{
        moves.clear();
        for (int i = src.y + 1, j = src.x + 1; i < 8 && j < 8; i++, j++) {
            if (board[i][j] == nullptr) moves.push_back(Coord(j, i));
            else {
                if (board[i][j]->colour != this->colour) moves.push_back(Coord(j, i));
                break;
            }
        }
        for (int i = src.y - 1, j = src.x - 1; i >= 0 && j >= 0; i--, j--) {
            if (board[i][j] == nullptr) moves.push_back(Coord(j, i));
            else {
                if (board[i][j]->colour != this->colour) moves.push_back(Coord(j, i));
                break;
            }
        }
        for (int i = src.x + 1, j = src.y - 1; i < 8 && j >= 0; i++, j--) {
            if (board[j][i] == nullptr) moves.push_back(Coord(i, j));
            else {
                if (board[j][i]->colour != this->colour) moves.push_back(Coord(i, j));
                break;
            }
        }
        for (int i = src.x - 1, j = src.y + 1; i >= 0 && j < 8; i--, j++) {
            if (board[j][i] == nullptr) moves.push_back(Coord(i, j));
            else {
                if (board[j][i]->colour != this->colour) moves.push_back(Coord(i, j));
                break;
            }
        }
    }
};

class Rook : public Piece {
public:
    int getValue() const { return 5; }
    bool hasMoved = false;
    Rook(string c, string name, char sym, Coord pos) : Piece(c, name, sym, pos) {}
    bool canMove(Coord src, Coord dst) override {
        
        if (src.x != dst.x && src.y != dst.y) return false;
        
        if (src.x == dst.x) {
            int minY = min(src.y, dst.y);
            int maxY = max(src.y, dst.y);
            for (int i = minY + 1; i < maxY; i++) {
                if (board[i][src.x] != nullptr) return false; 
            }
        } else {
            int minX = min(src.x, dst.x);
            int maxX = max(src.x, dst.x);
            for (int i = minX + 1; i < maxX; i++) {
                if (board[src.y][i] != nullptr) return false;
            }
        }
        
        return true;
    }
    void findMoves(Coord src) override{
        moves.clear();
        for (int i = src.y + 1; i < 8; i++) {
            if (board[i][src.x] == nullptr) moves.push_back(Coord(src.x, i));
            else {
                if (board[i][src.x]->colour != this->colour) moves.push_back(Coord(src.x, i));
                break;
            }
        }
        for (int i = src.y - 1; i >= 0; i--) {
            if (board[i][src.x] == nullptr) moves.push_back(Coord(src.x, i));
            else {
                if (board[i][src.x]->colour != this->colour) moves.push_back(Coord(src.x, i));
                break;
            }
        }
        for (int i = src.x + 1; i < 8; i++) {
            if (board[src.y][i] == nullptr) moves.push_back(Coord(i, src.y));
            else {
                if (board[src.y][i]->colour != this->colour) moves.push_back(Coord(i, src.y));
                break;
            }
        }
        for (int i = src.x - 1; i >= 0; i--) {
            if (board[src.y][i] == nullptr) moves.push_back(Coord(i, src.y));
            else {
                if (board[src.y][i]->colour != this->colour) moves.push_back(Coord(i, src.y));
                break;
            }
        }
    }
};

class King : public Piece {
public:
    bool hasMoved = false;
    int getValue() const { return 1000; }
    King(string c, string name, char sym, Coord pos) : Piece(c, name, sym, pos) {}
    bool canMove(Coord src, Coord dst) override {
        int X = abs(dst.x - src.x);
        int Y = abs(dst.y - src.y);
        if ((X == 1 && Y == 0) || (X == 0 && Y == 1) || (X == 1 && Y == 1)) {
            return true;
        }
        if (Y == 0 && X == 2 && !hasMoved) {
            int row = src.y;
            if (dst.x > src.x) {
                Piece* kRook = board[row][7];
                if (kRook != nullptr && kRook->name == "Rook"
                    && !static_cast<Rook*>(kRook)->hasMoved
                    && board[row][5] == nullptr
                    && board[row][6] == nullptr) return true;
            } else {
                Piece* qRook = board[row][0];
                if (qRook != nullptr && qRook->name == "Rook"
                    && !static_cast<Rook*>(qRook)->hasMoved
                    && board[row][1] == nullptr
                    && board[row][2] == nullptr
                    && board[row][3] == nullptr) return true;
            }
        }
        return false;
    }
    void findMoves(Coord src) override{
        moves.clear();
        Coord p(src.x, src.y);
        p.x = src.x + 1; p.y = src.y + 1;
        if (p.isValid()) if (board[p.y][p.x] == nullptr || board[p.y][p.x]->colour != this->colour) moves.push_back(p);
        p.x = src.x - 1; p.y = src.y + 1;
        if (p.isValid()) if (board[p.y][p.x] == nullptr || board[p.y][p.x]->colour != this->colour) moves.push_back(p);
        p.x = src.x + 1; p.y = src.y - 1;
        if (p.isValid()) if (board[p.y][p.x] == nullptr || board[p.y][p.x]->colour != this->colour) moves.push_back(p);
        p.x = src.x - 1; p.y = src.y - 1;
        if (p.isValid()) if (board[p.y][p.x] == nullptr || board[p.y][p.x]->colour != this->colour) moves.push_back(p);
        p.x = src.x + 1; p.y = src.y;
        if (p.isValid()) if (board[p.y][p.x] == nullptr || board[p.y][p.x]->colour != this->colour) moves.push_back(p);
        p.x = src.x - 1; p.y = src.y;
        if (p.isValid()) if (board[p.y][p.x] == nullptr || board[p.y][p.x]->colour != this->colour) moves.push_back(p);
        p.x = src.x; p.y = src.y + 1;
        if (p.isValid()) if (board[p.y][p.x] == nullptr || board[p.y][p.x]->colour != this->colour) moves.push_back(p);
        p.x = src.x; p.y = src.y - 1;
        if (p.isValid()) if (board[p.y][p.x] == nullptr || board[p.y][p.x]->colour != this->colour) moves.push_back(p);

        if (!hasMoved) {
            int row = src.y;
            Piece* kRook = board[row][7];
            if (kRook != nullptr && kRook->name == "Rook" 
                && !static_cast<Rook*>(kRook)->hasMoved
                && board[row][5] == nullptr 
                && board[row][6] == nullptr) {
                moves.push_back(Coord(src.x + 2, row));
            }

            Piece* qRook = board[row][0];
            if (qRook != nullptr && qRook->name == "Rook" 
                && !static_cast<Rook*>(qRook)->hasMoved
                && board[row][1] == nullptr 
                && board[row][2] == nullptr 
                && board[row][3] == nullptr) {
                moves.push_back(Coord(src.x - 2, row));
            }
        }
    }
};

class Queen : public Piece {
public:
    int getValue() const { return 9; }
    Queen(string c, string name, char sym, Coord pos) : Piece(c, name, sym, pos) {}
    bool canMove(Coord src, Coord dst) override {
        int X = abs(dst.x - src.x);
        int Y = abs(dst.y - src.y);
        if(X == Y && X!= 0 && Y != 0) {
            int stepX = (dst.x > src.x) ? 1 : -1;
            int stepY = (dst.y > src.y) ? 1 : -1;
            int currentX = src.x + stepX;
            int currentY = src.y + stepY;
            for(int i = 0; i < X - 1; i++) {
                if(board[currentY][currentX] != nullptr) return false;
                currentX += stepX;
                currentY += stepY;
            }
            return true;
        }
        else if (src.x == dst.x) {
            int minY = min(src.y, dst.y);
            int maxY = max(src.y, dst.y);
            for (int i = minY + 1; i < maxY; i++) {
                if (board[i][src.x] != nullptr) return false; 
            } 
            return true;
        }
        else if (src.y == dst.y) {
                int minX = min(src.x, dst.x);
                int maxX = max(src.x, dst.x);
                for (int i = minX + 1; i < maxX; i++) {
                    if (board[src.y][i] != nullptr) return false;
                }
                return true;
            }
            return false;
    }
    void findMoves(Coord src) override{
        moves.clear();
        for (int i = src.y + 1, j = src.x + 1; i < 8 && j < 8; i++, j++) {
            if (board[i][j] == nullptr) moves.push_back(Coord(j, i));
            else {
                if (board[i][j]->colour != this->colour) moves.push_back(Coord(j, i));
                break;
            }
        }
        for (int i = src.y - 1, j = src.x - 1; i >= 0 && j >= 0; i--, j--) {
            if (board[i][j] == nullptr) moves.push_back(Coord(j, i));
            else {
                if (board[i][j]->colour != this->colour) moves.push_back(Coord(j, i));
                break;
            }
        }
        for (int i = src.x + 1, j = src.y - 1; i < 8 && j >= 0; i++, j--) {
            if (board[j][i] == nullptr) moves.push_back(Coord(i, j));
            else {
                if (board[j][i]->colour != this->colour) moves.push_back(Coord(i, j));
                break;
            }
        }
        for (int i = src.x - 1, j = src.y + 1; i >= 0 && j < 8; i--, j++) {
            if (board[j][i] == nullptr) moves.push_back(Coord(i, j));
            else {
                if (board[j][i]->colour != this->colour) moves.push_back(Coord(i, j));
                break;
            }
        }
        for (int i = src.y + 1; i < 8; i++) {
            if (board[i][src.x] == nullptr) moves.push_back(Coord(src.x, i));
            else {
                if (board[i][src.x]->colour != this->colour) moves.push_back(Coord(src.x, i));
                break;
            }
        }
        for (int i = src.y - 1; i >= 0; i--) {
            if (board[i][src.x] == nullptr) moves.push_back(Coord(src.x, i));
            else {
                if (board[i][src.x]->colour != this->colour) moves.push_back(Coord(src.x, i));
                break;
            }
        }
        for(int i = src.x + 1; i < 8; i++) {
            if (board[src.y][i] == nullptr) moves.push_back(Coord(i, src.y));
            else {
                if (board[src.y][i]->colour != this->colour) moves.push_back(Coord(i, src.y));
                break;
            }
        }
        for(int i = src.x - 1; i >= 0; i--) {
            if (board[src.y][i] == nullptr) moves.push_back(Coord(i, src.y));
            else {
                if (board[src.y][i]->colour != this->colour) moves.push_back(Coord(i, src.y));
                break;
            }
        }
    }
};

class Pawn : public Piece {
public:
    int getValue() const { return 1; }
    Pawn(string c, string name, char sym, Coord pos) : Piece(c, name, sym, pos) {}
    bool canMove(Coord src, Coord dst) override {
        int X = abs(dst.x - src.x);
        int Y = dst.y - src.y;

        if (this->colour == "white") {
            if (Y == 2 && X == 0 && src.y == 1 && board[src.y + 1][src.x] == nullptr && board[dst.y][dst.x] == nullptr) return true;
            if (Y == 1 && X == 0 && board[dst.y][dst.x] == nullptr) return true;
            if (Y == 1 && X == 1 && board[dst.y][dst.x] != nullptr && board[dst.y][dst.x]->colour != this->colour) return true;
            if (Y == 1 && X == 1 && board[dst.y][dst.x] == nullptr
                && lastMove.piece == "Pawn"
                && lastMove.colour == "black"
                && abs(lastMove.from.y - lastMove.to.y) == 2
                && lastMove.to.x == dst.x
                && lastMove.to.y == src.y) return true;
        }
        else if (this->colour == "black") {
            if (Y == -2 && X == 0 && src.y == 6 && board[src.y - 1][src.x] == nullptr && board[dst.y][dst.x] == nullptr) return true;
            if (Y == -1 && X == 0 && board[dst.y][dst.x] == nullptr) return true;
            if (Y == -1 && X == 1 && board[dst.y][dst.x] != nullptr && board[dst.y][dst.x]->colour != this->colour) return true;
            if (Y == -1 && X == 1 && board[dst.y][dst.x] == nullptr
                && lastMove.piece == "Pawn"
                && lastMove.colour == "white"
                && abs(lastMove.from.y - lastMove.to.y) == 2
                && lastMove.to.x == dst.x
                && lastMove.to.y == src.y) return true;
        }
        return false;
    }
    void findMoves(Coord src) override {
        moves.clear();
        if (this->colour == "white") {
            if (src.y == 1 && board[src.y + 1][src.x] == nullptr && board[src.y + 2][src.x] == nullptr)
                moves.push_back(Coord(src.x, src.y + 2));
            if (src.y < 7 && board[src.y + 1][src.x] == nullptr)
                moves.push_back(Coord(src.x, src.y + 1));
            if (src.y < 7 && src.x < 7 && board[src.y + 1][src.x + 1] != nullptr && board[src.y + 1][src.x + 1]->colour != this->colour)
                moves.push_back(Coord(src.x + 1, src.y + 1));
            if (src.y < 7 && src.x > 0 && board[src.y + 1][src.x - 1] != nullptr && board[src.y + 1][src.x - 1]->colour != this->colour)
                moves.push_back(Coord(src.x - 1, src.y + 1));
            if (src.y < 7 && src.x < 7 && board[src.y + 1][src.x + 1] == nullptr
                && lastMove.piece == "Pawn"
                && lastMove.colour == "black"
                && abs(lastMove.from.y - lastMove.to.y) == 2
                && lastMove.to.x == src.x + 1
                && lastMove.to.y == src.y)
                moves.push_back(Coord(src.x + 1, src.y + 1));
            if (src.y < 7 && src.x > 0 && board[src.y + 1][src.x - 1] == nullptr
                && lastMove.piece == "Pawn"
                && lastMove.colour == "black"
                && abs(lastMove.from.y - lastMove.to.y) == 2
                && lastMove.to.x == src.x - 1
                && lastMove.to.y == src.y)
                moves.push_back(Coord(src.x - 1, src.y + 1));
        }
        else if (this->colour == "black") {
            if (src.y == 6 && board[src.y - 1][src.x] == nullptr && board[src.y - 2][src.x] == nullptr)
                moves.push_back(Coord(src.x, src.y - 2));
            if (src.y > 0 && board[src.y - 1][src.x] == nullptr)
                moves.push_back(Coord(src.x, src.y - 1));
            if (src.y > 0 && src.x < 7 && board[src.y - 1][src.x + 1] != nullptr && board[src.y - 1][src.x + 1]->colour != this->colour)
                moves.push_back(Coord(src.x + 1, src.y - 1));
            if (src.y > 0 && src.x > 0 && board[src.y - 1][src.x - 1] != nullptr && board[src.y - 1][src.x - 1]->colour != this->colour)
                moves.push_back(Coord(src.x - 1, src.y - 1));
            if (src.y > 0 && src.x < 7 && board[src.y - 1][src.x + 1] == nullptr
                && lastMove.piece == "Pawn"
                && lastMove.colour == "white"
                && abs(lastMove.from.y - lastMove.to.y) == 2
                && lastMove.to.x == src.x + 1
                && lastMove.to.y == src.y)
                moves.push_back(Coord(src.x + 1, src.y - 1));
            if (src.y > 0 && src.x > 0 && board[src.y - 1][src.x - 1] == nullptr
                && lastMove.piece == "Pawn"
                && lastMove.colour == "white"
                && abs(lastMove.from.y - lastMove.to.y) == 2
                && lastMove.to.x == src.x - 1
                && lastMove.to.y == src.y)
                moves.push_back(Coord(src.x - 1, src.y - 1));
        }
    }
};

string to_lower(string str) {
    for (int i = 0; i < str.length(); i++) {
        str[i] = tolower(str[i]);
    }
    return str;
}

void fillBoard() {
    for (int i = 2; i < 6; i++) {
        for (int j = 0; j < 8; j++) {
            board[i][j] = nullptr;
        }
    }
    board[0][0] = new Rook("white", "Rook", 'R', Coord(0, 0));
    board[0][1] = new Knight("white", "Knight", 'N', Coord(1, 0));
    board[0][2] = new Bishop("white", "Bishop", 'B', Coord(2, 0));
    board[0][3] = new Queen("white", "Queen", 'Q', Coord(3, 0));  
    board[0][4] = new King("white", "King", 'K', Coord(4, 0));    
    board[0][5] = new Bishop("white", "Bishop", 'B', Coord(5, 0));
    board[0][6] = new Knight("white", "Knight", 'N', Coord(6, 0));
    board[0][7] = new Rook("white", "Rook", 'R', Coord(7, 0));    
    for (int i = 0; i < 8; i++) {
        board[1][i] = new Pawn("white", "Pawn", 'P', Coord(i, 1));
        board[6][i] = new Pawn("black", "Pawn", 'p', Coord(i, 6));
    }
    board[7][0] = new Rook("black", "Rook", 'r', Coord(0, 7));    
    board[7][1] = new Knight("black", "Knight", 'n', Coord(1, 7));
    board[7][2] = new Bishop("black", "Bishop", 'b', Coord(2, 7));
    board[7][3] = new Queen("black", "Queen", 'q', Coord(3, 7));  
    board[7][4] = new King("black", "King", 'k', Coord(4, 7));    
    board[7][5] = new Bishop("black", "Bishop", 'b', Coord(5, 7));
    board[7][6] = new Knight("black", "Knight", 'n', Coord(6, 7));
    board[7][7] = new Rook("black", "Rook", 'r', Coord(7, 7));    
}

void displayBoard() {
    cout << endl;
    for (int i = 7; i >= 0; i--) {
        cout << DIM_GRAY << i + 1 << "  |  " << RESET;
        for (int j = 0; j < 8; j++) {
            if (board[i][j] == nullptr) {
                cout << DIM_GRAY << ".   " << RESET;
            } else {
                if (board[i][j]->colour == "white") {
                    cout << NEON_YELLOW << board[i][j]->symbol << RESET << "   ";
                } else {
                    cout << NEON_CYAN << board[i][j]->symbol << RESET << "   ";
                }
            }
        }
        cout << endl << endl;
    }
    cout << DIM_GRAY << "      a   b   c   d   e   f   g   h" << RESET << endl << endl << endl;
    cout << "Captured by White: " << NEON_CYAN;
    for (char c : whiteCaptured) {
        cout << c << " ";
    }
    cout << RESET << endl << endl;
    cout << "Captured by Black: " << NEON_YELLOW;
    for (char c : blackCaptured) {
        cout << c << " ";
    }
    cout << RESET << endl << endl;

    int advantage = materialAdvantage();
    if (advantage > 0)
        cout << NEON_YELLOW << "White is ahead by +" << advantage << RESET << endl;
    else if (advantage < 0)
        cout << NEON_CYAN << "Black is ahead by +" << abs(advantage) << RESET << endl;
    else
        cout << DIM_GRAY << "Material is equal" << RESET << endl;
}

bool sourceMoveValidate(Coord src) {
    if (board[src.y][src.x] == nullptr) {
        return false;
    }
    return true;
}

bool destMoveValidate(Coord dst, Coord src) {
    if (board[dst.y][dst.x] != nullptr && board[dst.y][dst.x] -> name == "King") {
        return false;
    }
    if ((board[dst.y][dst.x] == nullptr || board[dst.y][dst.x] -> colour != board[src.y][src.x] -> colour)) {
        return true;
    } else {
        return false;
    }
}

bool inBounds(Coord c) {
    return c.isValid();
}

bool takeMove(bool whiteTurn) {
    string currentColour = whiteTurn ? "white" : "black";

    if (isCheckmate(currentColour)) {
        cout << BRIGHT_RED << "CHECKMATE! " << (whiteTurn ? "Black" : "White") << " wins!" << RESET << endl;
        exit(0);
    }
    if (isStalemate(currentColour)) {
        cout << NEON_GREEN << "STALEMATE! It's a draw!" << RESET << endl;
        exit(0);
    }

    string move;
    cout << endl << (whiteTurn ? "White's turn: " : "Black's turn: ");
    getline(cin, move);

    if (move.length() < 4) {
        cout << BRIGHT_RED << "Invalid format! Use e2e4 style." << RESET << endl;
        return false;
    }

    int sourceX = move[0] - 'a';
    int sourceY = move[1] - '1';
    int destX   = move[2] - 'a';
    int destY   = move[3] - '1';

    Coord src(sourceX, sourceY), dst(destX, destY);

    if (!inBounds(src) || !inBounds(dst)) {
        cout << BRIGHT_RED << "RETRY: Invalid input! Please enter a valid move." << RESET << endl;
        return false;
    }

    Piece* movingPiece = board[src.y][src.x];

    if (movingPiece == nullptr) {
        cout << BRIGHT_RED << "There is no piece there!" << RESET << endl;
        return false;
    }

    string pieceName   = movingPiece->name;
    string pieceColour = movingPiece->colour;

    if (movingPiece->colour != currentColour) {
        cout << BRIGHT_RED << "That's not your piece!" << RESET << endl;
        return false;
    }

    if (src.x == dst.x && src.y == dst.y) {
        cout << BRIGHT_RED << "You must actually move the piece!" << RESET << endl;
        return false;
    }

    if (destMoveValidate(dst, src) && movingPiece->canMove(src, dst)) {

        if (pieceName == "King" && abs(dst.x - src.x) == 2) {
            if (isCheck(currentColour)) {
                cout << BRIGHT_RED << "Cannot castle while in check!" << RESET << endl;
                return false;
            }
            int passThroughX = (dst.x > src.x) ? src.x + 1 : src.x - 1;
            board[src.y][passThroughX] = movingPiece;
            board[src.y][src.x] = nullptr;
            bool throughCheck = isCheck(currentColour);
            board[src.y][src.x] = movingPiece;
            board[src.y][passThroughX] = nullptr;
            if (throughCheck) {
                cout << BRIGHT_RED << "Cannot castle through check!" << RESET << endl;
                return false;
            }
        }

        Piece* captured = board[dst.y][dst.x];

        board[dst.y][dst.x] = movingPiece;
        board[dst.y][dst.x]->pos = dst;
        board[src.y][src.x] = nullptr;

        bool isEnPassant = (pieceName == "Pawn" && captured == nullptr && abs(dst.y - src.y) == 1 && abs(dst.x - src.x) == 1);
        Piece* epCaptured = nullptr;
        if (isEnPassant) {
            epCaptured = board[src.y][dst.x];
            board[src.y][dst.x] = nullptr;
        }

        if (isCheck(currentColour)) {
            board[src.y][src.x] = movingPiece;
            board[src.y][src.x]->pos = src;
            board[dst.y][dst.x] = captured;
            if (isEnPassant) board[src.y][dst.x] = epCaptured;
            cout << BRIGHT_RED << "ILLEGAL MOVE! That leaves your king in check!" << RESET << endl;
            return false;
        }

        if (isEnPassant && epCaptured != nullptr) {
            (currentColour == "white") ? whiteCaptured.push_back(epCaptured->symbol) : blackCaptured.push_back(epCaptured->symbol);
            delete epCaptured;
        }

        if (captured != nullptr) {
            (currentColour == "white") ? whiteCaptured.push_back(captured->symbol) : blackCaptured.push_back(captured->symbol);
            delete captured;
        }

        if (pieceName == "King" && abs(dst.x - src.x) == 2) {
            int row = dst.y;
            if (dst.x > src.x) {
                board[row][5] = board[row][7];
                board[row][5]->pos = Coord(5, row);
                static_cast<Rook*>(board[row][5])->hasMoved = true;
                board[row][7] = nullptr;
            } else {
                board[row][3] = board[row][0];
                board[row][3]->pos = Coord(3, row);
                static_cast<Rook*>(board[row][3])->hasMoved = true;
                board[row][0] = nullptr;
            }
        }

        if (pieceName == "King") static_cast<King*>(movingPiece)->hasMoved = true;
        if (pieceName == "Rook") static_cast<Rook*>(movingPiece)->hasMoved = true;

        if (board[dst.y][dst.x] != nullptr && board[dst.y][dst.x]->name == "Pawn") {
            if ((whiteTurn && dst.y == 7) || (!whiteTurn && dst.y == 0)) {
                promotion(currentColour, dst);
            }
        }

        lastMove = {src, dst, pieceName, pieceColour};
        if (pieceName == "Pawn" || captured != nullptr || isEnPassant) {
            halfMoveClock = 0;
        } else {
            halfMoveClock++;
        }

        if (halfMoveClock >= 100) {
            cout << NEON_GREEN << "DRAW! Fifty move rule." << RESET << endl;
            exit(0);
        }

        string boardState = boardToString();
        positionHistory[boardState]++;
        if (positionHistory[boardState] >= 3) {
            cout << NEON_GREEN << "DRAW! Threefold repetition." << RESET << endl;
            exit(0);
        }
        return true;
    }

    cout << BRIGHT_RED << "Invalid move!" << RESET << endl;
    return false;
}

void promotion(string colour, Coord dst) {
    string choice;
    cout << NEON_GREEN << "Pawn promotion! Choose (Queen, Rook, Bishop, Knight): " << RESET;
    getline(cin, choice);
    choice = to_lower(choice);
    Piece* oldPawn = board[dst.y][dst.x];
    Piece* newPiece;
    if (choice == "rook") newPiece = new Rook(colour, "Rook", (colour == "white" ? 'R' : 'r'), dst);
    else if (choice == "bishop") newPiece = new Bishop(colour, "Bishop", (colour == "white" ? 'B' : 'b'), dst);
    else if (choice == "knight") newPiece = new Knight(colour, "Knight", (colour == "white" ? 'N' : 'n'), dst);
    else newPiece = new Queen(colour, "Queen", (colour == "white" ? 'Q' : 'q'), dst);
    board[dst.y][dst.x] = newPiece;
    delete oldPawn;
}

void cleanBoard() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] != nullptr) {
                delete board[i][j];
                board[i][j] = nullptr;
            }
        }
    }
}

bool isCheck(string colour) {
    Coord kingPos = findKing(colour);
    string opponentColour = (colour == "white") ? "black" : "white";
    
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] != nullptr && board[i][j]->colour == opponentColour) {
                board[i][j]->findMoves(board[i][j]->pos);
                for (auto& move : board[i][j]->moves) {
                    if (move.x == kingPos.x && move.y == kingPos.y) return true;
                }
            }
        }
    }
    return false;
}

Coord findKing(string colour) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] != nullptr && board[i][j]->colour == colour && board[i][j]->name == "King") {
                return Coord(j, i);
            }
        }
    }
    return Coord(-1, -1);
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

bool isCheckmate(string colour) {
    if(!isCheck(colour)) return false;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] != nullptr && board[i][j]->colour == colour) {
                board[i][j]->findMoves(board[i][j]->pos);
                for (auto& move : board[i][j]->moves) {
                    Coord piecePos(j, i);
                    Piece* temp = board[move.y][move.x];

                    board[move.y][move.x] = board[i][j];
                    board[move.y][move.x]->pos = move;
                    board[i][j] = nullptr;

                    bool simEnPassant = (board[move.y][move.x]->name == "Pawn" && temp == nullptr && abs(move.y - piecePos.y) == 1 && abs(move.x - piecePos.x) == 1);
                    Piece* simEpPawn = nullptr;
                    if (simEnPassant) {
                        simEpPawn = board[piecePos.y][move.x];
                        board[piecePos.y][move.x] = nullptr;
                    }

                    bool stillInCheck = isCheck(colour);

                    board[i][j] = board[move.y][move.x];
                    board[i][j]->pos = piecePos;
                    board[move.y][move.x] = temp;
                    if (simEnPassant) board[piecePos.y][move.x] = simEpPawn;

                    if (!stillInCheck) return false;
                }
            }
        }
    }
    return true;
}

bool isStalemate(string colour) {
    if (isCheck(colour)) return false;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] != nullptr && board[i][j]->colour == colour) {
                board[i][j]->findMoves(board[i][j]->pos);
                for (auto& move : board[i][j]->moves) {
                    Coord piecePos(j, i);
                    Piece* temp = board[move.y][move.x];

                    board[move.y][move.x] = board[i][j];
                    board[move.y][move.x]->pos = move;
                    board[i][j] = nullptr;

                    bool simEnPassant = (board[move.y][move.x]->name == "Pawn" && temp == nullptr && abs(move.y - piecePos.y) == 1 && abs(move.x - piecePos.x) == 1);
                    Piece* simEpPawn = nullptr;
                    if (simEnPassant) {
                        simEpPawn = board[piecePos.y][move.x];
                        board[piecePos.y][move.x] = nullptr;
                    }

                    bool stillInCheck = isCheck(colour);

                    board[i][j] = board[move.y][move.x];
                    board[i][j]->pos = piecePos;
                    board[move.y][move.x] = temp;
                    if (simEnPassant) board[piecePos.y][move.x] = simEpPawn;

                    if (!stillInCheck) return false;
                }
            }
        }
    }
    return true;
}

string boardToString() {
    string s = "";
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] == nullptr) {
                s += ".";
            } else {
                s += board[i][j]->symbol;
                s += board[i][j]->colour[0];
            }
        }
    }
    return s;
}

int materialAdvantage() {
    int whiteMaterial = 0, blackMaterial = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] != nullptr && board[i][j]->name != "King") {
                if (board[i][j]->colour == "white") whiteMaterial += board[i][j]->getValue();
                else blackMaterial += board[i][j]->getValue();
            }
        }
    }
    return whiteMaterial - blackMaterial;
}

int main() {
    fillBoard();
    bool whiteTurn = true;
    string message = "Welcome to Chess! White starts.";
    while (true) {
        clearScreen();
        displayBoard(); 
        cout << "STATUS: " << message << endl;
        
        if (takeMove(whiteTurn)) {
            whiteTurn = !whiteTurn;
            string nextColour = whiteTurn ? "white" : "black";
            if (isCheck(nextColour)) {
                message = (whiteTurn ? "White" : "Black") + string(" is in CHECK!");
            } else {
                message = "Move successful!";
            }
        } else {
            cout << endl << "Press Enter to try again..." << endl;
            cin.get();
            message = "Invalid move! Try again.";
        }
    }
    cleanBoard();    
    return 0;
}
