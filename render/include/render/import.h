#pragma once
#include <memory>
#include <unordered_map>
namespace render{
namespace asset{

class SceneAsset;
class Manager{
public:
  SceneAsset* load(const std::string& path);
  const auto& assets(){return m_assets;}
private:
  std::unordered_map<std::string,SceneAsset*> m_assets;
};


}
}
