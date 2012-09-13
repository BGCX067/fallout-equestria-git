#ifndef  GAMETASK_HPP
# define GAMETASK_HPP

# include "level/level.hpp"
# include "worldmap/worldmap.hpp"
# include "gameui.hpp"
# include "saveuis.hpp"

class LevelTask
{
public:
  LevelTask(WindowFramework* window, PT(RocketRegion) rocket);  
  ~LevelTask();
  
  void                  MapOpenLevel(std::string name);
  void                  SetLevel(Level* level);
  AsyncTask::DoneStatus do_task();
  bool                  SaveGame(const std::string& savepath);
  bool                  LoadGame(const std::string& savepath);  
  bool                  OpenLevel(const std::string& savepath, const std::string& level);
  void                  ExitLevel(const std::string& savepath);
  bool                  CopySave(const std::string& savepath, const std::string& slotPath);
  
  void                  SaveToSlot(unsigned char slot);
  void                  LoadSlot(unsigned char slot);
  void                  Exit(Rocket::Core::Event&) { _continue = false; }
  
  // LEVEL EVENTS
  void                  LevelExitZone(const std::string& toLevel);
  void                  UiSaveGame(const std::string& slotPath);
  void                  UiLoadGame(const std::string& slotPath);

private:
  void                  LoadClicked(Rocket::Core::Event&);
  void                  SaveClicked(Rocket::Core::Event&);
  static bool           SaveLevel(Level* level, const std::string& name);
  Level*                LoadLevel(WindowFramework* window, GameUi& gameUi, const std::string& name, bool isSaveFile = false);  
  Level*                DoLoadLevel(void);
  
  void                  EraseSlot(unsigned char slot);

  bool                  _continue;
  WindowFramework*      _window;
  GameUi                _gameUi;
  DataEngine            _dataEngine;
  
  WorldMap*             _worldMap;
  
  std::string           _levelName;
  Level*                _level;

  std::string           _savePath;
  
  UiSave*               _uiSaveGame;
  UiLoad*               _uiLoadGame;

  struct LoadLevelParams
  { LoadLevelParams() : doLoad(false) {} bool doLoad; std::string name; std::string path; bool isSaveFile; };

  LoadLevelParams       _loadLevelParams;
};

#endif