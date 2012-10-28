#ifndef  GAMETASK_HPP
# define GAMETASK_HPP

# include "level/level.hpp"
# include "worldmap/worldmap.hpp"
# include "playerparty.hpp"
# include "gameui.hpp"
# include "saveuis.hpp"
# include "statsheet.hpp"
# include "pipbuck.hpp"

class Buff
{
public:
  Buff(const std::string& name, StatController* stats, Data data, TimeManager& tm);
  Buff(Utils::Packet& packet,   TimeManager& tm, std::function<StatController* (const std::string&)>);
  ~Buff(void);

  const std::string& GetTargetName(void) const        { return (_target_name);  }
  StatController*    GetStatistics(void) const        { return (_target_stats); }
  void               SetStatistics(StatController* v) { _target_stats = v;      }

  void               Refresh(void);
  void               Save(Utils::Packet&);

  Observatory::Signal<void (Buff*)> Over;

private:
  void               InitScripts(void);
  
  asIScriptContext*  _context;
  asIScriptModule*   _module;
  asIScriptFunction* _refresh;

  Data               _buff;
  std::string        _target_name;
  StatController*    _target_stats;
  TimeManager&       _tm;
  TimeManager::Task* _task;
  bool               _looping;
};

struct BuffManager
{
  typedef std::list<Buff*> Buffs;
  
  BuffManager(TimeManager& tm) : tm(tm) {}
  ~BuffManager() { CollectGarbage(); }

  void  Save(Utils::Packet&, std::function<bool            (const std::string&)>);
  void  Load(Utils::Packet&, std::function<StatController* (const std::string&)>);

  Buffs        buffs;
  TimeManager& tm;
  
  void CollectGarbage(void);

private:
  void Cleanup(Buff*);

  Buffs garbage;
};

class GameTask
{
public:
  static GameTask* CurrentGameTask;
  GameTask(WindowFramework* window, GeneralUi&);
  ~GameTask();
  
  void                  MapOpenLevel(std::string name);
  void                  SetLevel(Level* level);
  AsyncTask::DoneStatus do_task();
  bool                  SaveGame(const std::string& savepath);
  bool                  LoadGame(const std::string& savepath);  
  void                  OpenLevel(const std::string& savepath, const std::string& level);
  void                  ExitLevel(const std::string& savepath);
  bool                  CopySave(const std::string& savepath, const std::string& slotPath);
  
  void                  SaveToSlot(unsigned char slot);
  void                  LoadSlot(unsigned char slot);
  void                  LoadLastState(void);
  void                  Exit(Rocket::Core::Event&) { _continue = false; }
  
  // LEVEL EVENTS
  void                  UiSaveGame(const std::string& slotPath);
  void                  UiLoadGame(const std::string& slotPath);

  // BUFF MANAGEMENT
  void                  PushBuff(ObjectCharacter* character, Data buff);
  void                  PushBuff(const std::string& name,    Data buff);
  void                  SaveLevelBuffs(Utils::Packet&);
  void                  SavePartyBuffs(Utils::Packet&);
  void                  LoadLevelBuffs(Utils::Packet&);
  void                  LoadPartyBuffs(Utils::Packet&);
  std::function<bool (const std::string&)> _is_level_buff;

private:
  void                  FinishLoad(void);
  void                  LoadClicked(Rocket::Core::Event&);
  void                  SaveClicked(Rocket::Core::Event&);
  static bool           SaveLevel(Level* level, const std::string& name);
  Level*                LoadLevel(WindowFramework* window, GameUi& gameUi, const std::string& path, const std::string& name, bool isSaveFile = false);  
  Level*                DoLoadLevel(void);
  void                  GameOver(void);
  
  void                  SetPlayerInventory(void);
  
  void                  EraseSlot(unsigned char slot);

  bool                  _continue;
  WindowFramework*      _window;
  GameUi                _gameUi;
  DataEngine            _dataEngine;
  TimeManager           _timeManager;
  BuffManager           _buff_manager;
  Pipbuck               _pipbuck;
  
  DataTree*             _charSheet;
  PlayerParty*          _playerParty;
  StatController*       _playerStats;
  Inventory*            _playerInventory;
  
  WorldMap*             _worldMap;
  
  std::string           _levelName;
  Level*                _level;

  std::string           _savePath;
  
  UiSave*               _uiSaveGame;
  UiLoad*               _uiLoadGame;

  struct LoadLevelParams
  { LoadLevelParams() : doLoad(false) {} bool doLoad; std::string name; std::string path; bool isSaveFile; std::string entry_zone; };

  LoadLevelParams       _loadLevelParams;
};

#endif