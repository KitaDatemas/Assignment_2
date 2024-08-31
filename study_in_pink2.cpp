#include "study_in_pink2.h"

ElementType MapElement::getType () const    {return type;}

///  BEGIN OF MODIFIED PATH CLASS
Path::Path () : MapElement (ElementType(PATH)) {};
///  END OF MODIFIED PATH CLASS



///  BEGIN OF MODIFIED WALL CLASS
Wall::Wall () : MapElement (ElementType(WALL)) {};
///  END OF MODIFIED WALL CLASS



///  BEGIN OF MODIFIED FAKEWALL CLASS
FakeWall::FakeWall (int in_reg_exp) : MapElement (ElementType(FAKE_WALL)), req_exp(in_reg_exp) {};

int FakeWall::getReqExp () const    {return req_exp;}
///  END OF MODIFIED FAKEWALL CLASS



///  BEGIN OF MODIFIED MAP CLASS
Map::Map (int num_rows, int num_cols, int num_walls, Position *array_walls, int num_fake_walls,
         Position *array_fake_walls) : num_cols(num_cols), num_rows(num_rows) {
    map = new MapElement ** [num_rows];
    for (int i = 0; i < num_rows; i++){
        map[i] = new MapElement * [num_cols];
    }

    for (int i = 0; i < num_rows; i++){
        for (int j = 0; j < num_cols; j++){
            map[i][j] = new Path;
        }
    }

    for (int i = 0; i < num_walls; i++){
        delete map[array_walls[i].getRow()][array_walls[i].getCol()];
        map [array_walls[i].getRow()][array_walls[i].getCol()] = new Wall;
    }

    for (int i = 0; i < num_fake_walls; i++){
        delete map[array_fake_walls[i].getRow()][array_fake_walls[i].getCol()];
        map[array_fake_walls[i].getRow()][array_fake_walls[i].getCol()] = new FakeWall((array_fake_walls[i].getRow() * 257 + array_fake_walls[i].getCol() * 139 + 89)%900 + 1);
    }
}

Map::~Map (){}

bool Map::isValid (const Position &pos, MovingObject *mv_obj) const {
    if (pos.getRow() >= num_rows || pos.getCol() >= num_cols ||
        pos.getRow() < 0 || pos.getCol() < 0)                       return false;
    if (map[pos.getRow()][pos.getCol()]->getType() == WALL)         return false;
    if (mv_obj->getName() == "Watson" &&
        map[pos.getRow()][pos.getCol()]->getType() == FAKE_WALL &&
        dynamic_cast<Watson *>(mv_obj)->getExp() <= dynamic_cast<FakeWall *>(map[pos.getRow()][pos.getCol()])->getReqExp())
        return false;
    return true;
}

MapElement ***Map::getMap ()    {return map;}
///  END OF MODIFIED MAP CLASS



///  BEGIN OF MODIFIED POSITION CLASS
const Position Position::npos = Position(-1, -1);

Position::Position (int r, int c) : r(r), c(c){};

Position::Position (const std::string &str_pos) {
    string str = str_pos.substr(1, str_pos.length() - 1);
    str = str.substr(0, str.length() - 1);
    for (auto i = str.begin(); i != str.end(); i++){
        if (*i == ','){
            *i = ' ';
            break;
        }
    }
    stringstream ss;
    ss << str;
    ss >> str;
    r = stoi(str);
    ss >> str;
    c = stoi(str);
}

int Position::getRow () const       {return r;}
int Position::getCol () const       {return c;}
void Position::setRow (int r)       {this->r = r;}
void Position::setCol (int c)       {this->c = c;}

string Position::str () const       {return "(" + to_string(r) + "," + to_string(c) + ")";}

bool Position::isEqual (int in_r, int in_c) const   {return (r == in_r) & (c == in_c);}

bool Position::isEqual (const Position &pos)        {return (r == pos.getRow()) & (c == pos.getCol());}
///  END OF MODIFIED POSITION CLASS


Character::Character (int index, const Position pos, Map *map, const std::string &name) : MovingObject (index, pos, map, name) {};


///  BEGIN OF MODIFIED MOVING_OBJECT CLASS
MovingObject::MovingObject (int index, const Position pos, Map *map, const std::string &name) : index(index), pos(pos), map(map), name(name), step(0) {};

MovingObject::~MovingObject () noexcept             {step = 0;}

Position MovingObject::getCurrentPosition () const  {return pos;}

string MovingObject::getName ()                     {return name;}
///  BEGIN OF MODIFIED MOVING_OBJECT CLASS



///  BEGIN OF MODIFIED WATSON CLASS
Watson::Watson (int index, const std::string &moving_rule, const Position &init_pos, Map *map, int init_hp,
                int init_exp) : moving(moving_rule), Character(index, init_pos, map, "Watson"){
    hp = (init_hp > 500)? 500 : init_hp;
    exp = (init_exp > 900)? 900 : init_exp;
};

Position Watson::getNextPosition (){
    if (exp <= 0)       return Position::npos;
    bool canMove = true;
    Position nextStep;
    switch(moving[step]){
        case 'L':
            if (map->isValid(Position(pos.getRow(), pos.getCol() - 1) , this) && (
                map->getMap()[pos.getRow()][pos.getCol() - 1]->getType() != WALL ||
                (map->getMap()[pos.getRow()][pos.getCol() - 1]->getType() == FAKE_WALL &&
                exp > dynamic_cast<FakeWall *>(map->getMap()[pos.getRow()][pos.getCol() - 1])->getReqExp()))){
                nextStep = Position(pos.getRow(), pos.getCol() - 1);
            } else nextStep = Position::npos;
            break;
        case 'R':
            if (map->isValid(Position(pos.getRow(), pos.getCol() + 1) , this) && (
                map->getMap()[pos.getRow()][pos.getCol() + 1]->getType() != WALL ||
                (map->getMap()[pos.getRow()][pos.getCol() + 1]->getType() == FAKE_WALL &&
                exp > dynamic_cast<FakeWall *>(map->getMap()[pos.getRow()][pos.getCol() + 1])->getReqExp()))){
                nextStep = Position(pos.getRow(), pos.getCol() + 1);
            } else nextStep = Position::npos;
            break;
        case 'U':
            if (map->isValid(Position(pos.getRow() - 1, pos.getCol()) , this) && (
                map->getMap()[pos.getRow() - 1][pos.getCol()]->getType() != WALL ||
                (map->getMap()[pos.getRow() - 1][pos.getCol()]->getType() == FAKE_WALL &&
                exp > dynamic_cast<FakeWall *>(map->getMap()[pos.getRow() - 1][pos.getCol()])->getReqExp()))){
                nextStep = Position(pos.getRow() - 1, pos.getCol());
            } else nextStep = Position::npos;
            break;
        case 'D':
            if (map->isValid(Position(pos.getRow() + 1, pos.getCol()) , this) && (
                map->getMap()[pos.getRow() + 1][pos.getCol()]->getType() != WALL ||
                (map->getMap()[pos.getRow() + 1][pos.getCol()]->getType() == FAKE_WALL &&
                exp > dynamic_cast<FakeWall *>(map->getMap()[pos.getRow() + 1][pos.getCol()])->getReqExp()))){
                nextStep = Position(pos.getRow() + 1, pos.getCol());
            } else nextStep = Position::npos;
            break;
    }
    return nextStep;
}

void Watson::move (){
    step = (step >= moving.length() || step < 0)? 0 : step;//Make sure that index is always available
    if (!getNextPosition().isEqual(Position::npos))     pos = getNextPosition();
    step = (step + 1 >= moving.length())? 0 : step + 1;
}

string Watson::str () const{
    return "Watson[index=" +
            to_string(index) +
            ";pos=" +
            pos.str() +
            ";moving_rule=" +
            moving +
            "]";
}

int Watson::getHp ()                {return hp;}

void Watson::setHp (int Hp)         {hp = (Hp > 500)? 500 : Hp;}

int Watson::getExp ()               {return exp;}

void Watson::setExp (int Exp)       {exp = (Exp > 900)? 900 : Exp;}


void Watson::setPos (Position pos)  {this->pos = pos;}
///  END OF MODIFIED WATSON CLASS



///  BEGIN OF MODIFIED SHERLOCK CLASS
Sherlock::Sherlock (int index,
                   const string &moving_rule,
                   const Position &init_pos,
                   Map *map,
                   int init_hp,
                   int init_exp) :

                   moving(moving_rule),
                   Character(index,
                            init_pos,
                            map,
                            "Sherlock"){

    hp = (init_hp > 500)? 500 : init_hp;
    exp = (init_exp > 900)? 900 : init_exp;
};

Position Sherlock::getNextPosition (){
    if (exp < 0)            return Position::npos;
    Position nextStep;
    switch(moving[step]){
        case 'L':
            if (map->isValid(Position(pos.getRow(), pos.getCol() - 1) , this) &&
                map->getMap()[pos.getRow()][pos.getCol() - 1]->getType() != WALL){
                nextStep = Position(pos.getRow(), pos.getCol() - 1);
            } else nextStep = Position::npos;
            break;
        case 'R':
            if (map->isValid(Position(pos.getRow(), pos.getCol() + 1) , this) &&
                map->getMap()[pos.getRow()][pos.getCol() + 1]->getType() != WALL){
                nextStep = Position(pos.getRow(), pos.getCol() + 1);
            } else nextStep = Position::npos;
            break;
        case 'U':
            if (map->isValid(Position(pos.getRow() - 1, pos.getCol()) , this) &&
                map->getMap()[pos.getRow() - 1][pos.getCol()]->getType() != WALL){
                nextStep = Position(pos.getRow() - 1, pos.getCol());
            } else nextStep = Position::npos;
            break;
        case 'D':
            if (map->isValid(Position(pos.getRow() + 1, pos.getCol()) , this) &&
                map->getMap()[pos.getRow() + 1][pos.getCol()]->getType() != WALL){
                nextStep = Position(pos.getRow() + 1, pos.getCol());
            } else nextStep = Position::npos;
            break;
    }
    return nextStep;
}

void Sherlock::move (){
    step = (step >= moving.length() || step < 0)? 0 : step;//Make sure index is always available
    if (!getNextPosition().isEqual(Position::npos))         pos = getNextPosition();
    step = ++step >= moving.length()? 0 : step;
}

string Sherlock::str () const           {return "Sherlock[index=" + to_string(index) + ";pos=" + pos.str() + ";moving_rule=" + moving + "]";}

int Sherlock::getHp ()                  {return hp;}

void Sherlock::setHp (int Hp)           {hp = (Hp > 500)? 500 : Hp;}

int Sherlock::getExp ()                 {return exp;}

void Sherlock::setExp (int Exp)         {exp = (Exp > 900)? 900 : Exp;}

void Sherlock::setPos (Position pos)    {this->pos = pos;}

//Sherlock::~Sherlock () noexcept         {}
///  END OF MODIFIED SHERLOCK CLASS



///  BEGIN OF MODIFIED CRIMINAL CLASS
Criminal::Criminal (int index, const Position & init_pos, Map *map, Sherlock * sherlock, Watson * watson) : sherlock(sherlock), watson(watson), step(0),
                                                                                                           prevPos(Position::npos), Character(index, init_pos, map, "Criminal"){};

Position Criminal::getNextPosition (){
    int distance = -1;
    Position maxPos;
    for (int i = 0; i < 4; i++){
        int x, y;
        switch (i){
            case 0://Up case
                x = -1;
                y = 0;
                break;
            case 1://Left case
                x = 0;
                y = -1;
                break;
            case 2://Down case
                x = 1;
                y = 0;
                break;
            case 3://Right case
                x = 0;
                y = 1;
                break;
        }
        if (map->isValid( Position(pos.getRow() + x, pos.getCol() + y), this)) {
            int dis = abs(pos.getRow() + x - sherlock->getCurrentPosition().getRow()) +
                      abs(pos.getCol() + y - sherlock->getCurrentPosition().getCol()) +
                      abs(pos.getRow() + x - watson->getCurrentPosition().getRow()) +
                      abs(pos.getCol() + y - watson->getCurrentPosition().getCol());
            if (dis > distance) {
                distance = dis;
                maxPos = Position(pos.getRow() + x, pos.getCol() + y);
            }
        }
    }
    return maxPos;
}

void Criminal::move (){
    Position nextPos = getNextPosition();
    if (!nextPos.isEqual(Position::npos)) {
        prevPos = pos;
        pos = nextPos;
    }
}

string Criminal::str () const{
    return "Criminal[index=" +
    to_string(index) +
    ";pos=" + pos.str() +
    "]";
}

Position Criminal::getPrevPos () {return prevPos;}
///  END OF MODIFIED CRIMINAL CLASS



///  BEGIN OF MODIFIED ARRAY_MOVING_OBJECT CLASS
ArrayMovingObject::ArrayMovingObject (int capacity) : count(0), capacity(capacity) {arr_mv_objs = new MovingObject * [capacity];}

ArrayMovingObject::~ArrayMovingObject ()    {delete []arr_mv_objs;}

bool ArrayMovingObject::isFull () const     {return !(capacity - count);}

bool ArrayMovingObject::add (MovingObject *mv_obj) {
    if (isFull())               return false;
    arr_mv_objs[count++] = mv_obj;
    return true;
}

string ArrayMovingObject::str () const {
    string str_re = "ArrayMovingObject[count=" + to_string(count) + ";capacity=" + to_string(capacity);
    for (int i = 0; i < count; i++){
        str_re += ";" + arr_mv_objs[i]->str();
    }
    return str_re + "]";
}

int ArrayMovingObject::size () const    {return count;}

MovingObject * ArrayMovingObject::get (int index) const {
    if (index < 0 || index >= count)        return nullptr;
    return arr_mv_objs[index];
}
///  END OF MODIFIED ARRAY_MOVING_OBJECT CLASS



///  BEGIN OF MODIFIED CONFIGURATION CLASS
Configuration::Configuration (const std::string &filepath) {
    ifstream fs(filepath);
    while (!fs.eof()){
        string str, action;
        fs >> str;
        action = takeAction(str);
        if (action == "MAP_NUM_ROWS")                   map_num_rows = stoi(str);
        else if (action == "MAP_NUM_COLS")              map_num_cols = stoi(str);
        else if (action == "MAX_NUM_MOVING_OBJECTS")    max_num_moving_objects = stoi(str);
        else if (action == "SHERLOCK_MOVING_RULE")      sherlock_moving_rule = str;
        else if (action == "SHERLOCK_INIT_POS")         sherlock_init_pos = Position(str);
        else if (action == "SHERLOCK_INIT_HP")          sherlock_init_hp = stoi(str);
        else if (action == "SHERLOCK_INIT_EXP")         sherlock_init_exp = stoi(str);
        else if (action == "WATSON_MOVING_RULE")        watson_moving_rule = str;
        else if (action == "WATSON_INIT_POS")           watson_init_pos = Position(str);
        else if (action == "WATSON_INIT_HP")            watson_init_hp = stoi(str);
        else if (action == "WATSON_INIT_EXP")           watson_init_exp = stoi(str);
        else if (action == "CRIMINAL_INIT_POS")         criminal_init_pos = Position(str);
        else if (action == "NUM_STEPS")                 num_steps = stoi(str);
        else if (action == "ARRAY_WALLS"){
            num_walls = 0;
            str = str.substr(1, str.length() - 2);
            for (int i = 0; i < str.length(); i++){
                if (str[i] == ';')                      num_walls++;
            }
            if (str.length() > 0)                       num_walls++;
            if (num_walls > 0)                          arr_walls = new Position[num_walls];
            else                                        arr_walls = nullptr;
            for (int i = 0; i < num_walls; i++){
                arr_walls[i] = getNextPos(str);
            }
        }
        else if (action == "ARRAY_FAKE_WALLS"){
            num_fake_walls = 0;
            str = str.substr(1, str.length() - 2);
            for (int i = 0; i < str.length(); i++){
                if (str[i] == ';')                      num_fake_walls++;
            }
            if (str.length() > 0)                       num_fake_walls++;
            if (num_fake_walls > 0)                     arr_fake_walls = new Position[num_fake_walls];
            else                                        arr_fake_walls = nullptr;
            for (int i = 0; i < num_fake_walls; i++){
                arr_fake_walls[i] = getNextPos(str);
            }
        }
    }
    fs.close();
}

Configuration::~Configuration () {
    delete []arr_walls;
    delete []arr_fake_walls;
}

string Configuration::str () const {
    string config =   "Configuration[\n"
            "MAP_NUM_ROWS=" + to_string(map_num_rows) + "\n"
            "MAP_NUM_COLS=" + to_string(map_num_cols) + "\n"
            "MAX_NUM_MOVING_OBJECTS=" + to_string(max_num_moving_objects) + "\n"
            "NUM_WALLS=" + to_string(num_walls) + "\n"
            "ARRAY_WALLS=[";
            for (int i = 0; i < num_walls; i++){
                if (i != 0)             config += ';';
                config += arr_walls[i].str();
            }

            config += "]\nNUM_FAKE_WALLS=" + to_string(num_fake_walls) + "\n"
            "ARRAY_FAKE_WALLS=[";
            for (int i = 0; i < num_fake_walls; i++){
                if (i != 0)             config += ';';
                config += arr_fake_walls[i].str();
            }
            config += "]\nSHERLOCK_MOVING_RULE=" + sherlock_moving_rule + "\n"
            "SHERLOCK_INIT_POS=" + sherlock_init_pos.str() + "\n"
            "SHERLOCK_INIT_HP=" + to_string(sherlock_init_hp) + "\n"
            "SHERLOCK_INIT_EXP=" + to_string(sherlock_init_exp) + "\n"
            "WATSON_MOVING_RULE=" + watson_moving_rule + "\n"
            "WATSON_INIT_POS=" + watson_init_pos.str() + "\n"
            "WATSON_INIT_HP=" + to_string(watson_init_hp) + "\n"
            "WATSON_INIT_EXP=" + to_string(watson_init_exp) + "\n"
            "CRIMINAL_INIT_POS=" + criminal_init_pos.str() + "\n"
            "NUM_STEPS=" + to_string(num_steps) + "\n]";
    return config;
}

int Configuration::intConfig (const std::string &attr) {
    if (attr == "map_num_rows")                     return map_num_rows;
    else if (attr == "map_num_cols")                return map_num_cols;
    else if (attr == "max_num_moving_objects")      return max_num_moving_objects;
    else if (attr == "num_walls")                   return num_walls;
    else if (attr == "sherlock_init_hp")            return sherlock_init_hp;
    else if (attr == "sherlock_init_exp")           return sherlock_init_exp;
    else if (attr == "watson_init_hp")              return watson_init_hp;
    else if (attr == "watson_init_exp")             return watson_init_exp;
    else if (attr == "num_steps")                   return num_steps;
    return -1;
}

Position * Configuration::posConfig (const std::string &attr) {
    if (attr == "arr_walls")                        return arr_walls;
    else if (attr == "arr_fake_walls")              return arr_fake_walls;
    else if (attr == "sherlock_init_pos")           return &sherlock_init_pos;
    else if (attr == "watson_init_pos")             return &watson_init_pos;
    else if (attr == "criminal_init_pos")           return &criminal_init_pos;
    return nullptr;
}

string Configuration::strConfig (const std::string &attr) {
    if (attr == "sherlock_moving_rule")             return sherlock_moving_rule;
    else if (attr == "watson_moving_rule")          return watson_moving_rule;
    return "";
}
///  END OF MODIFIED CONFIGURATION CLASS


Robot::Robot (int index, const Position pos, Map *map, const std::string &name) : MovingObject(index, pos, map, name) {};


///  BEGIN OF MODIFIED ROBOTC CLASS
RobotC::RobotC (int index, const Position &init_pos, Map *map, Criminal *criminal) : robot_type(C), criminal(criminal), item(
        nullptr), Robot(index, init_pos, map, "RobotC"){
    int RulingNumber = init_pos.getRow() * init_pos.getCol();
    while (to_string(RulingNumber).length() > 1) {
        int sum = 0;
        for (int i = 0; i < to_string(RulingNumber).length(); i++) {
            sum += to_string(RulingNumber)[i] - '0';
        }
        RulingNumber = sum;
    }
    if (RulingNumber < 2)               item = new MagicBook;
    else if (RulingNumber < 4)          item = new EnergyDrink;
    else if (RulingNumber < 6)          item = new FirstAid;
    else if (RulingNumber < 8)          item = new ExcemptionCard;
    else {
        RulingNumber = (init_pos.getRow() * 11 + init_pos.getCol())%4;
        switch (RulingNumber){
            case 0:
                item = new PassingCard("RobotS");
                break;
            case 1:
                item = new PassingCard("RobotC");
                break;
            case 2:
                item = new PassingCard("RobotSW");
                break;
            case 3:
                item = new PassingCard("all");
                break;
        }
    }
};

Position RobotC::getNextPosition ()     {return criminal->getPrevPos();}

void RobotC::move () {
    if (!getNextPosition().isEqual(Position::npos)){
        pos = getNextPosition();
    }
}

int RobotC::getDistance (MovingObject * target) const{
    return abs(pos.getRow() - target->getCurrentPosition().getRow()) + abs(pos.getCol() - target->getCurrentPosition().getCol());
}

string RobotC::str () const         {return "Robot[pos=" + pos.str() + ";type=C;dist=]";}

BaseItem * RobotC::getItem ()       {return item;}
///  END OF MODIFIED ROBOTC CLASS



///  BEGIN OF MODIFIED ROBOTS CLASS
RobotS::RobotS (int index, const Position &init_pos, Map * map, Criminal * criminal, Sherlock * sherlock)
                : robot_type(S), sherlock(sherlock), item(nullptr), Robot(index, init_pos, map, "RobotS"){

    int RulingNumber = init_pos.getRow() * init_pos.getCol();
    while (to_string(RulingNumber).length() > 1) {
        int sum = 0;
        for (int i = 0; i < to_string(RulingNumber).length(); i++) {
            sum += to_string(RulingNumber)[i] - '0';
        }
        RulingNumber = sum;
    }
    if (RulingNumber < 2)               item = new MagicBook;
    else if (RulingNumber < 4)          item = new EnergyDrink;
    else if (RulingNumber < 6)          item = new FirstAid;
    else if (RulingNumber < 8)          item = new ExcemptionCard;
    else {
        RulingNumber = (init_pos.getRow() * 11 + init_pos.getCol())%4;
        switch (RulingNumber){
            case 0:
                item = new PassingCard("RobotS");
                break;
            case 1:
                item = new PassingCard("RobotC");
                break;
            case 2:
                item = new PassingCard("RobotSW");
                break;
            case 3:
                item = new PassingCard("all");
                break;
        }
    }
};

Position RobotS::getNextPosition () {
    int distance = 2147483647;
    Position minPos = Position::npos;
    for (int i = 0; i < 4; i++){
        int x, y;
        switch (i){
            case 0://Up case
                x = -1;
                y = 0;
                break;
            case 1://Right case
                x = 0;
                y = 1;
                break;
            case 2://Down case
                x = 1;
                y = 0;
                break;
            case 3://Left case
                x = 0;
                y = -1;
                break;
        }
        if (map->isValid( Position(pos.getRow() + x, pos.getCol() + y), this) &&
            map->getMap()[pos.getRow() + x][pos.getCol() + y]->getType() != WALL) {
            int dis = abs(pos.getRow() + x - sherlock->getCurrentPosition().getRow()) +
                      abs(pos.getCol() + y - sherlock->getCurrentPosition().getCol());
            if (dis < distance) {
                distance = dis;
                minPos = Position(pos.getRow() + x, pos.getCol() + y);
            }
        }
    }
    return minPos;
}

void RobotS::move () {
    if (!getNextPosition().isEqual(Position::npos)){
        pos = getNextPosition();
    }
}

int RobotS::getDistance () const{
    return abs(pos.getRow() - sherlock->getCurrentPosition().getRow()) + abs(pos.getCol() - sherlock->getCurrentPosition().getCol());
}

string RobotS::str () const{
    return "Robot[pos=" + pos.str() + ";type=S;dist=" + to_string(getDistance()) + "]";
}

BaseItem * RobotS::getItem () {
    BaseItem * robotItem;
    switch (item->getItemType()){
        case MAGIC_BOOK:
            robotItem = new MagicBook;
            break;
        case ENERGY_DRINK:
            robotItem = new EnergyDrink;
            break;
        case FIRST_AID:
            robotItem = new FirstAid;
            break;
        case EXCEMPTION_CARD:
            robotItem = new ExcemptionCard;
            break;
        case PASSING_CARD:
            robotItem = new PassingCard(dynamic_cast<PassingCard *>(item)->getChallenge());
    }
    return robotItem;
}
///  END OF MODIFIED ROBOTS CLASS



///  BEGIN OF MODIFIED ROBOTW CLASS
RobotW::RobotW (int index, const Position &init_pos, Map * map, Criminal * criminal, Watson * watson) : robot_type(S), watson(watson), item(
        nullptr), Robot(index, init_pos, map, "RobotW"){

    int RulingNumber = init_pos.getRow() * init_pos.getCol();
    while (to_string(RulingNumber).length() > 1) {
        int sum = 0;
        for (int i = 0; i < to_string(RulingNumber).length(); i++) {
            sum += to_string(RulingNumber)[i] - '0';
        }
        RulingNumber = sum;
    }
    if (RulingNumber < 2)               item = new MagicBook;
    else if (RulingNumber < 4)          item = new EnergyDrink;
    else if (RulingNumber < 6)          item = new FirstAid;
    else if (RulingNumber < 8)          item = new ExcemptionCard;
    else {
        RulingNumber = (init_pos.getRow() * 11 + init_pos.getCol())%4;
        switch (RulingNumber){
            case 0:
                item = new PassingCard("RobotS");
                break;
            case 1:
                item = new PassingCard("RobotC");
                break;
            case 2:
                item = new PassingCard("RobotSW");
                break;
            case 3:
                item = new PassingCard("all");
                break;
        }
    }
};

Position RobotW::getNextPosition () {
    int distance = 2147483647;
    Position minPos = Position::npos;
    for (int i = 0; i < 4; i++){
        int x, y;
        switch (i){
            case 0://Up case
                x = -1;
                y = 0;
                break;
            case 1://Right case
                x = 0;
                y = 1;
                break;
            case 2://Down case
                x = 1;
                y = 0;
                break;
            case 3://Left case
                x = 0;
                y = -1;
                break;
        }
        if (map->isValid( Position(pos.getRow() + x, pos.getCol() + y), this)) {
            int dis = abs(pos.getRow() + x - watson->getCurrentPosition().getRow()) +
                      abs(pos.getCol() + y - watson->getCurrentPosition().getCol());
            if (dis < distance &&
                map->getMap()[pos.getRow() + x][pos.getCol() + y]->getType() != WALL) {
                distance = dis;
                minPos = Position(pos.getRow() + x, pos.getCol() + y);
            }
        }
    }
    return minPos;
}

void RobotW::move () {
    if (!getNextPosition().isEqual(Position::npos)){
        pos = getNextPosition();
    }
}

int RobotW::getDistance () const{
    return abs(pos.getRow() - watson->getCurrentPosition().getRow()) + abs(pos.getCol() - watson->getCurrentPosition().getCol());
}

string RobotW::str () const{
    return "Robot[pos=" + pos.str() + ";type=W;dist=" + to_string(getDistance()) + "]";
}

BaseItem * RobotW::getItem () {
    BaseItem * robotItem;
    switch (item->getItemType()){
        case MAGIC_BOOK:
            robotItem = new MagicBook;
            break;
        case ENERGY_DRINK:
            robotItem = new EnergyDrink;
            break;
        case FIRST_AID:
            robotItem = new FirstAid;
            break;
        case EXCEMPTION_CARD:
            robotItem = new ExcemptionCard;
            break;
        case PASSING_CARD:
            robotItem = new PassingCard(dynamic_cast<PassingCard *>(item)->getChallenge());
    }
    return robotItem;
}
///  END OF MODIFIED ROBOTW CLASS



///  BEGIN OF MODIFIED ROBOTSW CLASS
RobotSW::RobotSW (int index, const Position &init_pos, Map * map, Criminal * criminal, Sherlock * sherlock, Watson * watson) : robot_type(S), sherlock(sherlock), watson(watson), item(
        nullptr), Robot(index, init_pos, map, "RobotSW"){

    int RulingNumber = init_pos.getRow() * init_pos.getCol();
    while (to_string(RulingNumber).length() > 1) {
        int sum = 0;
        for (int i = 0; i < to_string(RulingNumber).length(); i++) {
            sum += to_string(RulingNumber)[i] - '0';
        }
        RulingNumber = sum;
    }
    if (RulingNumber < 2)               item = new MagicBook;
    else if (RulingNumber < 4)          item = new EnergyDrink;
    else if (RulingNumber < 6)          item = new FirstAid;
    else if (RulingNumber < 8)          item = new ExcemptionCard;
    else {
        RulingNumber = (init_pos.getRow() * 11 + init_pos.getCol())%4;
        switch (RulingNumber){
            case 0:
                item = new PassingCard("RobotS");
                break;
            case 1:
                item = new PassingCard("RobotC");
                break;
            case 2:
                item = new PassingCard("RobotSW");
                break;
            case 3:
                item = new PassingCard("all");
                break;
        }
    }
};

Position RobotSW::getNextPosition () {
    int distance = 2147483647;
    Position minPos = Position::npos;
    for (int i = 0; i < 8; i++){
        int x, y;
        switch (i){
            case 0://Up case
                x = -2;
                y = 0;
                break;
            case 1://Up left case
                x = -1;
                y = -1;
            case 2://Left case
                x = 0;
                y = -2;
                break;
            case 3://Left Down case
                x = 1;
                y = -1;
                break;
            case 4://Down case
                x = 2;
                y = 0;
                break;
            case 5://Down right
                x = 1;
                y = 1;
                break;
            case 6://Right
                x = 0;
                y = 2;
                break;
            case 7://Right Up
                x = -1;
                y = 1;
                break;
        }
        if (map->isValid( Position(pos.getRow() + x, Robot::pos.getCol() + y), this)) {
            int disSherlock = abs(pos.getRow() + x - sherlock->getCurrentPosition().getRow()) +
                            abs(pos.getCol() + y - sherlock->getCurrentPosition().getCol());
            int disWatson = abs(pos.getRow() + x - watson->getCurrentPosition().getRow()) +
                            abs(pos.getCol() + y - watson->getCurrentPosition().getCol());
            if (disWatson + disSherlock < distance &&
                map->getMap()[pos.getRow() + x][pos.getCol() + y]->getType() != WALL) {
                distance = disWatson + disSherlock;
                minPos = Position(Robot::pos.getRow() + x, Robot::pos.getCol() + y);
            }
        }
    }
    return minPos;
}

void RobotSW::move () {
    if (!getNextPosition().isEqual(Position::npos)){
        pos = getNextPosition();
    }
}

int RobotSW::getDistance () const{
    return abs(pos.getRow() - sherlock->getCurrentPosition().getRow()) + abs(pos.getCol() - sherlock->getCurrentPosition().getCol()) +
        abs(pos.getRow() - watson->getCurrentPosition().getRow()) + abs(pos.getCol() - watson->getCurrentPosition().getCol());
}

string RobotSW::str () const{
    return "Robot[pos=" + pos.str() + ";type=SW;dist=" + to_string(getDistance()) + "]";
}

BaseItem * RobotSW::getItem () {
    BaseItem * robotItem;
    switch (item->getItemType()){
        case MAGIC_BOOK:
            robotItem = new MagicBook;
            break;
        case ENERGY_DRINK:
            robotItem = new EnergyDrink;
            break;
        case FIRST_AID:
            robotItem = new FirstAid;
            break;
        case EXCEMPTION_CARD:
            robotItem = new ExcemptionCard;
            break;
        case PASSING_CARD:
            robotItem = new PassingCard(dynamic_cast<PassingCard *>(item)->getChallenge());
    }
    return robotItem;
}
///  END OF MODIFIED ROBOTSW CLASS



///  BEGIN OF MODIFIED MAGICBOOK CLASS
bool MagicBook::canUse (Character *character, Robot *robot) {
    if (robot)                          return false;
    if (!character)                     return false;

    if (character->getName() == "Sherlock" &&
        dynamic_cast<Sherlock *> (character)->getExp() > 350)
                                        return false;
    if (character->getName() == "Watson" &&
        dynamic_cast<Watson *> (character)->getExp() > 350)
                                        return false;
    return true;
}

void MagicBook::use (Character *character, Robot *robot) {
    if (!canUse(character, robot))              return;
    if (character->getName() == "Sherlock"){
        dynamic_cast<Sherlock *>(character)->setExp(dynamic_cast<Sherlock *>(character)->getExp() * 1.25);
    } else{
        dynamic_cast<Watson *>(character)->setExp(dynamic_cast<Watson *>(character)->getExp() * 1.25);
    }
}
///  END OF MODIFIED MAGICBOOK CLASS



///  BEGIN OF MODIFIED ENERGYDRINK CLASS
bool EnergyDrink::canUse (Character *character, Robot *robot) {
    if (robot)                          return false;
    if (!character)                     return false;

    if (character->getName() == "Sherlock" &&
        dynamic_cast<Sherlock *> (character)->getHp() > 100)
        return false;
    if (character->getName() == "Watson" &&
        dynamic_cast<Watson *> (character)->getHp() > 100)
        return false;
    return true;
}

void EnergyDrink::use (Character *character, Robot *robot) {
    if (!canUse(character, robot))      return;
    if (character->getName() == "Sherlock"){
        dynamic_cast<Sherlock *>(character)->setHp(dynamic_cast<Sherlock *>(character)->getHp() * 1.2);
    } else{
        dynamic_cast<Watson *>(character)->setHp(dynamic_cast<Watson *>(character)->getHp() * 1.2);
    }
}
///  END OF MODIFIED ENERGYDRINK CLASS



///  BEGIN OF MODIFIED FIRSTAID CLASS
bool FirstAid::canUse (Character *character, Robot *robot) {
    if (robot)                          return false;
    if (!character)                     return false;

    if (character->getName() == "Sherlock" &&
        dynamic_cast<Sherlock *> (character)->getHp() > 100 &&
        dynamic_cast<Sherlock *> (character)->getExp() > 350)
        return false;
    if (character->getName() == "Watson" &&
        dynamic_cast<Watson *> (character)->getHp() > 100 &&
        dynamic_cast<Watson *> (character)->getExp() > 350)
        return false;
    return true;
}

void FirstAid::use (Character *character, Robot *robot) {
    if (!canUse(character, robot))      return;
    if (character->getName() == "Sherlock"){
        dynamic_cast<Sherlock *>(character)->setHp(dynamic_cast<Sherlock *>(character)->getHp() * 1.5);
    } else{
        dynamic_cast<Watson *>(character)->setHp(dynamic_cast<Watson *>(character)->getHp() * 1.5);
    }
}
///  END OF MODIFIED FIRSTAID CLASS



///  BEGIN OF MODIFIED EXEPTIONCARD CLASS
bool ExcemptionCard::canUse (Character *character, Robot *robot) {
    if (!robot)                             return false;
    if (character->getName() == "Watson")   return false;

    if (character->getName() == "Sherlock" &&
        dynamic_cast<Sherlock *>(character)->getHp()%2 == 1)
        return true;
    return false;
}

void ExcemptionCard::use (Character *character, Robot *robot) {
    if (!canUse(character, robot))             return;
}
///  END OF MODIFIED EXCEMPTIONCARD CLASS



///  BEGIN OF MODIFIED PASSINGCARD CLASS
PassingCard::PassingCard (std::string challenge) : challenge(challenge) {}

bool PassingCard::canUse (Character *character, Robot *robot) {
    if (!robot)                                 return false;
    if (character->getName() == "Sherlock")     return false;

    if (character->getName() == "Watson" &&
        dynamic_cast<Watson *>(character)->getHp()%2 == 0)
        return true;
    return false;
}

void PassingCard::use (Character *character, Robot *robot) {
    if (!canUse(character, robot))             return;
    if (challenge != robot->getName() && challenge != "all"){
        int hp = dynamic_cast<Watson *>(character)->getExp() - 50;
        hp = (hp < 0)? 0 : hp;
        dynamic_cast<Watson *>(character)->setExp(hp);
    }
}

string PassingCard::getChallenge () {return challenge;}
///  END OF MODIFIED PASSINGCARD CLASS



///  BEGIN OF MODIFIED SHERLOCKBAG CLASS
SherlockBag::SherlockBag (Sherlock * sherlock) {
    BagTop = nullptr;
    obj = sherlock;
    count = 0;
    capacity = 13;
}

bool SherlockBag::insert (BaseItem *item) {
    if (count >= capacity)          return false;
    NodeBag * newNode = new NodeBag(item, BagTop);
    BagTop = newNode;
    count++;
    return true;
}

BaseItem * SherlockBag::get () {
    NodeBag * p = BagTop;
    BaseItem * re = nullptr;
    while (p){
        if ((p->item->getItemType() == MAGIC_BOOK ||
             p->item->getItemType() == ENERGY_DRINK ||
             p->item->getItemType() == FIRST_AID)
            && p->item->canUse(obj, nullptr)){
            NodeBag * del = BagTop;
            re = p->item;
            p->item = BagTop->item;
            BagTop = BagTop->next;
            count--;
            delete del;
            break;
        }
        p = p->next;
    }
    return re;
}

BaseItem * SherlockBag::get (ItemType itemType) {
    NodeBag * p = BagTop;
    while (p && p->item->getItemType() != itemType)     {
        p = p->next;
    }
    if (!p)                     return nullptr;
    BaseItem * re = p->item;
    p->item = BagTop->item;
    NodeBag * del = BagTop;
    BagTop = BagTop->next;
    delete del;
    count--;
    return re;
}

string SherlockBag::str () const{
    string re = "Bag[count=" + to_string(count);
    NodeBag * p = BagTop;
    for (int  i = 0; i < count; i++){
        if (i == 0)             re += ";";
        else                    re += ",";
        switch (p->item->getItemType()){
            case MAGIC_BOOK:
                re += "MagicBook";
                break;
            case ENERGY_DRINK:
                re += "EnergyDrink";
                break;
            case FIRST_AID:
                re += "FirstAid";
                break;
            case EXCEMPTION_CARD:
                re += "ExcemptionCard";
                break;
            case PASSING_CARD:
                re += "PassingCard";
        }
    }
    return re + "]";
}
///  END OF MODIFIED SHERLOCKBAG CLASS



///  BEGIN OF MODIFIED WATSONBAG CLASS
WatsonBag::WatsonBag (Watson * watson) {
    BagTop = nullptr;
    obj = watson;
    count = 0;
    capacity = 13;
}

bool WatsonBag::insert (BaseItem *item) {
    if (count >= capacity)          return false;
    NodeBag *newNode = new NodeBag(item, BagTop);
    BagTop = newNode;
    count++;
    return true;
}

BaseItem * WatsonBag::get () {
    NodeBag * p = BagTop;
    BaseItem * re = nullptr;
    NodeBag * del = BagTop;
    while (p){
        if ((p->item->getItemType() == MAGIC_BOOK ||
            p->item->getItemType() == ENERGY_DRINK ||
            p->item->getItemType() == FIRST_AID)
            && p->item->canUse(obj, nullptr)){
            re = p->item;
            p->item = BagTop->item;
            BagTop = BagTop->next;
            count--;
            delete del;
            break;
        }
        p = p->next;
    }
    return re;
}

BaseItem * WatsonBag::get (ItemType itemType) {
    NodeBag * p = BagTop;
    while (p && p->item->getItemType() != itemType)     p = p->next;
    if (!p)                         return nullptr;
    BaseItem * re = p->item;
    p->item = BagTop->item;
    NodeBag * del = BagTop;
    BagTop = BagTop->next;
    delete del;
    count--;
    return re;
}

string WatsonBag::str () const{
    string re = "Bag[count=" + to_string(count);
    NodeBag * p = BagTop;
    for (int  i = 0; i < count; i++){
        if (i == 0)             re += ";";
        else                    re += ",";
        switch (p->item->getItemType()){
            case MAGIC_BOOK:
                re += "MagicBook";
                break;
            case ENERGY_DRINK:
                re += "EnergyDrink";
                break;
            case FIRST_AID:
                re += "FirstAid";
                break;
            case EXCEMPTION_CARD:
                re += "ExcemptionCard";
                break;
            case PASSING_CARD:
                re += "PassingCard";
        }
    }
    return re + "]";
}
///  END OF MODIFIED WATSONBAG CLASS



/// BEGIN OF MODIFIED STUDYINPINK CLASS
StudyPinkProgram::StudyPinkProgram (const std::string &config_file_path) : config(new Configuration(config_file_path)) {
    map = new Map(config->map_num_rows, config->map_num_cols,
                  config->num_walls,  config->arr_walls,
                  config->num_fake_walls, config->arr_fake_walls);

    sherlock = new Sherlock(0, config->sherlock_moving_rule, config->sherlock_init_pos, map, config->sherlock_init_hp, config->sherlock_init_exp);
    watson = new Watson(0, config->watson_moving_rule, config->watson_init_pos, map, config->watson_init_hp, config->watson_init_exp);
    criminal = new Criminal(0, config->criminal_init_pos, map, sherlock, watson);

    arr_mv_objs = new ArrayMovingObject(config->max_num_moving_objects);
    arr_mv_objs->add(criminal);
    arr_mv_objs->add(sherlock);
    arr_mv_objs->add(watson);

    sherlockBag = new SherlockBag(sherlock);
    watsonBag = new WatsonBag(watson);
    stopFlag = false;
}

StudyPinkProgram::~StudyPinkProgram () {
    delete sherlock;
    delete watson;
    delete criminal;
    delete sherlockBag;
    delete watsonBag;
    delete arr_mv_objs;
}

bool StudyPinkProgram::isStop () const {
    if (stopFlag)                                               return true;
    if (sherlock->getHp() <= 1 || watson->getHp() <= 1)         return false;
    if (sherlock->getCurrentPosition().isEqual(criminal->getCurrentPosition()) || watson->getCurrentPosition().isEqual(criminal->getCurrentPosition()))
        return true;
    return false;
}

void StudyPinkProgram::run (bool verbose) {
    for (int istep = 0; istep < config->num_steps; ++istep) {
        for (int i = 0; i < arr_mv_objs->size(); ++i) {
            arr_mv_objs->get(i)->move();
            if (verbose && i < 3) {
                printStep(istep);
            }
            if (arr_mv_objs->get(i)->getName() == "Sherlock" || arr_mv_objs->get(i)->getName() == "Watson") {
                if (sherlock->getCurrentPosition().isEqual(watson->getCurrentPosition())){
                    swap(sherlockBag, watsonBag);
                }
                for (int j = 3; j < arr_mv_objs->size(); j++){
                    if (arr_mv_objs->get(j)->getCurrentPosition().isEqual(arr_mv_objs->get(i)->getCurrentPosition())){
                        if (arr_mv_objs->get(i)->getName() == "Sherlock"){
                            string name = arr_mv_objs->get(j)->getName();
                            if (name == "RobotS"){
                                BaseItem * item = nullptr;
                                if (sherlock->getHp()%2 == 1)       item = sherlockBag->get(EXCEMPTION_CARD);
                                if (sherlock->getExp() > 400) {
                                    sherlockBag->insert(dynamic_cast<RobotS *>(arr_mv_objs->get(j))->getItem());
                                    item = sherlockBag->get();
                                    if (item)           item->use(sherlock, nullptr);
                                }
                                else if (item) {/*NOT LOOSING HP OR EXP*/}
                                else    sherlock->setExp(sherlock->getExp()*0.9);
                            }
                            else if (name == "RobotW")          sherlockBag->insert(dynamic_cast<RobotW *>(arr_mv_objs->get(j))->getItem());
                            else if (name == "RobotSW"){
                                BaseItem * item = nullptr;
                                if (sherlock->getHp()%2 == 1)       item = sherlockBag->get(EXCEMPTION_CARD);
                                if (sherlock->getHp() > 335 && sherlock->getExp() > 300) {
                                    sherlockBag->insert(dynamic_cast<RobotSW *>(arr_mv_objs->get(j))->getItem());
                                    item = sherlockBag->get();
                                    if (item)           item->use(sherlock, nullptr);
                                } else if (item) {/*USING EXCEMPTION_CARD*/}
                                else{
                                    sherlock->setHp(sherlock->getHp()*0.85);
                                    sherlock->setExp(sherlock->getExp()*0.85);
                                }
                            }
                            else if (name == "RobotC"){
                                if (sherlock->getExp() > 500){
                                    sherlock->setPos(criminal->getCurrentPosition());
                                    stopFlag = true;
                                    BaseItem * item = sherlockBag->get();
                                    if (item)       item->use(sherlock, nullptr);
                                } else              sherlockBag->insert(dynamic_cast<RobotC *>(arr_mv_objs->get(j))->getItem());
                            }
                        }else{
                            string name = arr_mv_objs->get(j)->getName();
                            if (name == "RobotS"){/*DO NOTHING*/}
                            else if (name == "RobotW"){
                                BaseItem * item = nullptr;
                                if (watson->getHp()%2 == 0)         item = watsonBag->get(PASSING_CARD);
                                if (watson->getHp() > 350) {
                                    watsonBag->insert(dynamic_cast<RobotW *>(arr_mv_objs->get(j))->getItem());
                                    item = watsonBag->get();
                                    if (item)         item->use(watson, nullptr);
                                } else if (item)      item->use(watson, dynamic_cast<RobotW *>(arr_mv_objs->get(j)));
                                else                  watson->setHp(watson->getHp()*0.95);
                            } else if (name == "RobotSW"){
                                BaseItem * item = nullptr;
                                if (watson->getHp()%2 == 0)         item = watsonBag->get(PASSING_CARD);
                                if (watson->getExp() > 600 && watson->getHp() > 165){
                                    watsonBag->insert(dynamic_cast<RobotSW *>(arr_mv_objs->get(j))->getItem());
                                    item = watsonBag->get();
                                    if (item)       item->use(watson, nullptr);
                                }
                                else if (item)      item->use(watson, dynamic_cast<RobotSW *>(arr_mv_objs->get(j)));
                                else{
                                    watson->setHp(watson->getHp()*0.85);
                                    watson->setExp(watson->getExp()*0.85);
                                }
                            }
                            else if (name == "RobotC"){
                                watsonBag->insert(dynamic_cast<RobotC *>(arr_mv_objs->get(j))->getItem());
                                BaseItem * item = watsonBag->get();
                                if (item)       item->use(watson, nullptr);
                            }
                        }
                        if (verbose)            printStep(istep);
                        if (isStop()) {
                            return;
                        }
                    }
                }
            }
            if ((istep + 1) % 3 == 0 && arr_mv_objs->get(i)->getName() == "Criminal"){
                if (!arr_mv_objs->isFull()){
                    Robot *newRobot;
                    int distance = (abs(criminal->getCurrentPosition().getRow() - sherlock->getCurrentPosition().getRow()) +
                                   abs(criminal->getCurrentPosition().getCol() - sherlock->getCurrentPosition().getCol())) -
                                   (abs(criminal->getCurrentPosition().getRow() - watson->getCurrentPosition().getRow()) +
                                   abs(criminal->getCurrentPosition().getCol() - watson->getCurrentPosition().getCol()));
                    if (istep == 2)             newRobot = new RobotC(0, criminal->getPrevPos(), map, criminal);
                    else if (distance < 0)      newRobot = new RobotS(0, criminal->getPrevPos(), map, criminal, sherlock);
                    else if (distance > 0)      newRobot = new RobotW(0, criminal->getPrevPos(), map, criminal, watson);
                    else                        newRobot = new RobotSW(0, criminal->getPrevPos(), map, criminal, sherlock, watson);
                    arr_mv_objs->add(newRobot);
                }
            }
        }
    }
}
/// END OF MODIFIED STUDYINPINK CLASS


string takeAction (string &str){
    string action;
    for (int i = 0; i < str.length(); i++){
        if (str[i] == '='){
            action = str.substr(0, i);
            str = str.substr(i + 1, str.length() - i - 1);
            return action;
        }
    }
    return "invalid";
}


Position getNextPos (string &str){
    string secondnum;
    for (int i = 0; i < str.length(); i++){
        if (str[i] == ';' || i == str.length() - 1){
            if (str[i] == ';')          secondnum = str.substr(1, i - 2);
            else                        secondnum = str.substr(1, i - 1);
            str = str.substr(i + 1, str.length() - i - 1);
            string firstnum = "";
            for (int i = 0; i < secondnum.length(); i++){
                if (secondnum[i] != ',')        firstnum += secondnum[i];
                else {
                    secondnum = secondnum.substr(i + 1, secondnum.length() - i - 1);
                    break;
                }
            }
            return Position(stoi(firstnum), stoi(secondnum));
        }
    }
    return Position::npos;
}

bool swap (SherlockBag * sherlockBag, WatsonBag * watsonBag){
    bool canSwap = false;
    BaseItem * item;
    while (item = sherlockBag->get(PASSING_CARD)){
        watsonBag->insert(item);
        canSwap = true;
    }
    while (item = watsonBag->get(EXCEMPTION_CARD)){
        sherlockBag->insert(item);
        canSwap = true;
    }
    return canSwap;
}
