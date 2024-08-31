#ifndef _H_STUDY_IN_PINK_2_H_
#define _H_STUDY_IN_PINK_2_H_

#include "main.h"

// Forward declaration
 class MovingObject;
 class Position;
 class Configuration;
 class Map;

 class Character;
 class Criminal;
 class Robot;
 class RobotS;
 class RobotW;
 class RobotSW;
 class RobotC;

 class ArrayMovingObject;
 class StudyPinkProgram;

 class BaseItem;
 class BaseBag;
 class SherlockBag;
 class WatsonBag;

class TestStudyInPink;

enum ItemType { MAGIC_BOOK, ENERGY_DRINK, FIRST_AID, EXCEMPTION_CARD, PASSING_CARD };
enum ElementType { PATH, WALL, FAKE_WALL };
enum RobotType { C = 0, S, W, SW };

class MapElement {
protected:
    ElementType             type;

public:
    MapElement (ElementType in_type) : type(in_type){};
    virtual ~MapElement (){};

    virtual ElementType     getType () const;

    friend class TestStudyInPink;
};

class Path : public MapElement {
public:
    Path ();
    ~Path (){}

    ElementType        getType () const     {return type;}

    friend class TestStudyInPink;
};

class Wall : public MapElement {
public:
    Wall ();
    ~Wall (){};

    ElementType         getType () const     {return type;}

    friend class TestStudyInPink;
};

class FakeWall : public MapElement {
    int                 req_exp;//Minimum EXP for Watson to recognize this Fake Wall

public:
    FakeWall (int in_reg_exp);
    ~FakeWall (){};

    int                 getReqExp () const;
    ElementType         getType () const     {return type;}

    friend class TestStudyInPink;
};

class Map {
private:
    int                 num_rows, num_cols;
    MapElement ***      map;

public:
    Map (int num_rows, int num_cols, int num_walls, Position * array_walls, int num_fake_walls, Position * array_fake_walls);
    ~Map ();

    bool                isValid ( const Position & pos , MovingObject * mv_obj ) const ;
    MapElement ***      getMap ();

    friend class TestStudyInPink;
};

class Position {
private:
    int r, c;
public:
    static const Position npos;

    Position (int r=0, int c=0);
    Position (const string & str_pos);

    int         getRow () const;
    int         getCol () const;
    void        setRow (int r);
    void        setCol (int c);
    string      str () const;
    bool        isEqual (int in_r, int in_c) const;
    bool        isEqual (const Position &pos);

    friend class TestStudyInPink;
};

class MovingObject {
protected:
    int         index, step;
    Position    pos;
    Map *       map;
    string      name;

public:
    MovingObject (int index, const Position pos, Map * map, const string & name="");
    virtual ~MovingObject ();

    virtual Position        getNextPosition () = 0;
    Position                getCurrentPosition () const;
    virtual void            move () = 0;
    virtual string          str () const = 0;
    string                  getName ();

    friend class TestStudyInPink;
};

class Character : public MovingObject {
public:
    Character (int index, const Position pos, Map * map, const string & name="");
};

class Sherlock : public Character {
private:
    int         hp, exp;
    string      moving;

public:
    Sherlock (int index, const string & moving_rule, const Position & init_pos, Map * map, int init_hp, int init_exp);

    Position    getNextPosition ();
    void        move ();
    string      str () const;
    int         getHp ();
    void        setHp (int Hp);
    int         getExp ();
    void        setExp (int Exp);
    void        setPos (Position pos);

    friend class TestStudyInPink;
};

class Watson : public Character {
private:
    int         hp, exp;
    string      moving;

public:
    Watson (int index, const string & moving_rule, const Position & init_pos, Map * map, int init_hp, int init_exp);

    Position    getNextPosition ();
    void        move ();
    string      str () const;
    int         getHp ();
    void        setHp (int Hp);
    int         getExp ();
    void        setExp (int Exp);
    void        setPos (Position pos);

    friend class TestStudyInPink;
};

class Criminal : public Character {
private:
    int                 step;
    Sherlock *          sherlock;
    Watson *            watson;
    Position            prevPos;

public:
    Criminal (int index, const Position & init_pos, Map * map, Sherlock * sherlock, Watson * watson);
    Position            getNextPosition ();
    void                move ();
    string              str () const;
    Position            getPrevPos ();

    friend class TestStudyInPink;
};

class ArrayMovingObject {
public:
    MovingObject **     arr_mv_objs;
    int                 count;
    int                 capacity;

    ArrayMovingObject (int capacity);
    ~ArrayMovingObject ();

    bool                isFull () const;
    bool                add (MovingObject * mv_obj);
    MovingObject *      get (int index) const;
    int                 size () const; // return current number of elements in the array
    string              str () const;

    friend class TestStudyInPink;
};

class Configuration {
private:
    int         map_num_rows,
                map_num_cols,
                max_num_moving_objects,
                num_walls,
                num_fake_walls,
                sherlock_init_hp,
                sherlock_init_exp,
                watson_init_hp,
                watson_init_exp,
                num_steps;

    Position    *arr_walls,
                *arr_fake_walls,
                sherlock_init_pos,
                watson_init_pos,
                criminal_init_pos;

    string      sherlock_moving_rule,
                watson_moving_rule;

public:
    Configuration (const string & filepath);
    ~Configuration ();

    string          str () const;
    int             intConfig (const string &attr);
    Position *      posConfig (const string &attr);
    string          strConfig (const string &attr);

    friend class TestStudyInPink;
    friend class StudyPinkProgram;
};

class Robot : public MovingObject{
public:
    Robot (int index, const Position pos, Map * map, const string & name="");
    virtual BaseItem *      getItem () = 0;
};

class RobotC : public Robot{
    RobotType           robot_type;
    BaseItem *          item;
    Criminal *          criminal;

public:
    RobotC (int index, const Position & init_pos, Map * map, Criminal * criminal);

    Position            getNextPosition ();
    void                move ();
    int                 getDistance (MovingObject * target) const;
    string              str () const;
    BaseItem *          getItem ();

    friend class TestStudyInPink;
};

class RobotS : public Robot{
    RobotType           robot_type;
    BaseItem *          item;
    Sherlock *          sherlock;

public:
    RobotS (int index, const Position & init_pos, Map * map, Criminal * criminal, Sherlock * sherlock );

    Position            getNextPosition ();
    void                move ();
    int                 getDistance () const;
    string              str () const;
    BaseItem *          getItem ();

    friend class TestStudyInPink;
};

class RobotW : public Robot{
    RobotType           robot_type;
    BaseItem *          item;
    Watson *            watson;

public:
    RobotW (int index, const Position & init_pos, Map * map, Criminal * criminal, Watson * watson);

    Position            getNextPosition ();
    void                move ();
    int                 getDistance () const;
    string              str () const;
    BaseItem *          getItem ();

    friend class TestStudyInPink;
};

class RobotSW : public Robot{
    RobotType           robot_type;
    BaseItem *          item;
    Sherlock *          sherlock;
    Watson *            watson;

public:
    RobotSW (int index, const Position & init_pos, Map * map, Criminal * criminal, Sherlock * sherlock, Watson * watson);

    Position            getNextPosition ();
    void                move ();
    int                 getDistance () const;
    string              str () const;
    BaseItem *          getItem ();

    friend class TestStudyInPink;
};

class BaseItem{
protected:
    ItemType            type;

public:
    virtual bool        canUse (Character * obj, Robot * robot) = 0;
    virtual void        use (Character * obj, Robot * robot) = 0;
    virtual ItemType    getItemType () = 0;

    friend class TestStudyInPink;
};

class MagicBook : public BaseItem{
public:
    bool        canUse (Character * character, Robot * robot);
    void        use (Character * character, Robot * robot);
    ItemType    getItemType ()   {return MAGIC_BOOK;}

    friend class TestStudyInPink;
};

class EnergyDrink : public BaseItem{
public:
    bool        canUse (Character * character, Robot * robot);
    void        use (Character * character, Robot * robot);
    ItemType    getItemType ()   {return ENERGY_DRINK;}

    friend class TestStudyInPink;
};

class FirstAid : public BaseItem{
public:
    bool        canUse (Character * character, Robot * robot);
    void        use (Character * character, Robot * robot);
    ItemType    getItemType ()   {return FIRST_AID;}

    friend class TestStudyInPink;
};

class ExcemptionCard : public BaseItem{
public:
    bool        canUse (Character * character, Robot * robot);
    void        use (Character * character, Robot * robot);
    ItemType    getItemType ()   {return EXCEMPTION_CARD;}

    friend class TestStudyInPink;
};

class PassingCard : public BaseItem{
protected:
    string      challenge;
public:
    PassingCard (string challenge);

    bool        canUse (Character * character, Robot * robot);
    void        use (Character * character, Robot * robot);
    ItemType    getItemType ()   {return PASSING_CARD;}
    string      getChallenge ();

    friend class TestStudyInPink;
};

class BaseBag{
protected:
    struct NodeBag{
        BaseItem *      item;
        NodeBag *       next;
        NodeBag(BaseItem * item, NodeBag * next = nullptr){
            this->item = item;
            this->next = next;
        };
    } * BagTop;

    Character *         obj;
    int                 count, capacity;

public:
    virtual bool        insert (BaseItem * item)    {return true;} ; // return true if insertsuccessfully
    virtual BaseItem *  get ()                      {return nullptr;} ; // return the item as described above , if notfound , return NULL
    virtual BaseItem *  get (ItemType itemType)     {return nullptr;} ; // return the item as describedabove , if not found , return NULL
    virtual string      str () const                {return "";} ;
    int                 getCount ()                 {return count;}

    friend class TestStudyInPink;
};

class SherlockBag : public BaseBag{
protected:
    Sherlock *          sherlock;

public:
    SherlockBag (Sherlock * sherlock);
    bool                insert (BaseItem * item);
    BaseItem *          get () ; // return the item as described above , if notfound , return NULL
    BaseItem *          get (ItemType itemType) ; // return the item as describedabove , if not found , return NULL
    string              str () const ;

    friend class TestStudyInPink;
};

class WatsonBag : public BaseBag{
protected:
    Watson *            watson;

public:
    WatsonBag (Watson * watson);
    bool                insert (BaseItem * item);
    BaseItem *          get () ; // return the item as described above , if notfound , return NULL
    BaseItem *          get (ItemType itemType) ; // return the item as describedabove , if not found , return NULL
    string              str () const ;

    friend class TestStudyInPink;
};

class StudyPinkProgram {
private:
    Configuration *     config;
    Map *               map;
    ArrayMovingObject * arr_mv_objs;

    Sherlock *          sherlock;
    Watson *            watson;
    Criminal *          criminal;

    SherlockBag *       sherlockBag;
    WatsonBag *         watsonBag;
    bool                stopFlag;

public:
    StudyPinkProgram (const string & config_file_path);

    bool isStop () const;

    void printResult () const {
        if (sherlock->getCurrentPosition().isEqual(criminal->getCurrentPosition())) {
            cout << "Sherlock caught the criminal" << endl;
        }
        else if (watson->getCurrentPosition().isEqual(criminal->getCurrentPosition())) {
            cout << "Watson caught the criminal" << endl;
        }
        else {
            cout << "The criminal escaped" << endl;
        }
    }

    void printStep (int si) const {
        cout << "Step: " << setw(4) << setfill('0') << si
            << "--"
            << sherlock->str() << "--|--" << watson->str() << "--|--" << criminal->str() << endl;
    }

    void run (bool verbose);

    ~StudyPinkProgram();

    friend class TestStudyInPink;
};

string takeAction (string &str);

Position getNextPos (string &str);

bool swap (SherlockBag * sherlockBag, WatsonBag * watsonBag);

#endif /* _H_STUDY_IN_PINK_2_H_ */