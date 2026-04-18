#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include "SelectFilterPopup.hpp"

using namespace geode::prelude;

bool validObject(GameObject* obj, const std::vector<Filter>& filterVector) {
    if (filterVector.empty()) return true;

    std::unordered_map<std::string, bool> validTypesEquals {
        {"object id", false}, {"group", false}, {"layer", false}, 
        {"z order", false}, {"color", false}, {"scale", false}, 
        {"hsv", false}, {"type", false}
    };
    std::unordered_map<std::string, int> lastFilterIndex;
    
    for (int i = 0; i < filterVector.size(); i++) {
        lastFilterIndex[filterVector[i].type] = i;
    }
    
    int i = -1;

    auto validFilter = [&i, &validTypesEquals, &lastFilterIndex, &filterVector] 
                        (const Filter& filter, bool condition) -> bool {
        if (condition && !filter.notEquals) {
            validTypesEquals[filter.type] = true;
            return true;
        }
        else if (!condition && filter.notEquals) return true;
        else if (!filter.notEquals && i != lastFilterIndex[filter.type]) return true;
        else return false;
    };

    for (auto& filter : filterVector) {
        i++;
        if (validTypesEquals.at(filter.type)) continue;

        if (filter.type == "object id") {
            if (!validFilter(filter, obj->m_objectID == filter.intVal)) return false;
        }
        else if (filter.type == "group") {
            bool matched = false;
            if (obj->m_groups) {
                for (auto group : *obj->m_groups) {
                    if (group == filter.intVal) { 
                        matched = true; 
                        break; 
                    }
                }
            } 
            if (!validFilter(filter, matched)) return false;
        }
        else if (filter.type == "layer") {
            auto objLayer = static_cast<int>(obj->m_zLayer);
            if (objLayer == 0 && filter.intVal != 0) objLayer = static_cast<int>(obj->m_defaultZLayer);
            if (!validFilter(filter, objLayer == filter.intVal)) return false;
        }
        else if (filter.type == "z order") {
            auto objOrder = obj->m_zOrder;
            if (objOrder == 0 && filter.intVal != 0) objOrder = obj->m_defaultZOrder;
            if (!validFilter(filter, objOrder == filter.intVal)) return false;
        }
        else if (filter.type == "color") {
            std::vector<GJSpriteColor*> cols;
            for (auto col : {obj->m_baseColor, obj->m_detailColor}) if (col) cols.push_back(col);
            bool matched = false;
            for (auto col : cols) {
                if (col->m_colorID == filter.intVal || (col->m_colorID == 0 && col->m_defaultColorID == filter.intVal)) { 
                    matched = true; 
                    break; 
                }
            }
            if (!validFilter(filter, matched)) return false;
        }
        else if (filter.type == "scale") {
            auto matched = false;
            if (filter.typedFloatVal.type == 0 && (obj->m_scaleX == obj->m_scaleY ? obj->m_scaleX : -1.0f) == filter.typedFloatVal.value) matched = true;
            else if (filter.typedFloatVal.type == 1 && obj->m_scaleX == filter.typedFloatVal.value) matched = true;
            else if (filter.typedFloatVal.type == 2 && obj->m_scaleY == filter.typedFloatVal.value) matched = true;
            if (!validFilter(filter, matched)) return false;
        }
        else if (filter.type == "hsv") {
            std::vector<GJSpriteColor*> cols;
            for (auto col : {obj->m_baseColor, obj->m_detailColor}) if (col) cols.push_back(col);
            bool matched = false;
            for (auto col : cols) {
                if (!filter.notEquals && !col->m_usesHSV) continue;
                if (filter.typedFloatVal.type == 0 && col->m_hsv.h == filter.typedFloatVal.value) matched = true;
                else if (filter.typedFloatVal.type == 1 && col->m_hsv.s == filter.typedFloatVal.value) matched = true;
                else if (filter.typedFloatVal.type == 2 && col->m_hsv.v == filter.typedFloatVal.value) matched = true;
                if (matched) break;
            }
            if (!validFilter(filter, matched)) return false;
        }
        else if (filter.type == "type") {
            bool matched = false;
            if (filter.stringVal == "any") matched = true;
            else if (filter.stringVal == "layout" && !(obj->m_objectType == GameObjectType::Decoration)) matched = true;
            else if (filter.stringVal == "trigger" && obj->m_isTrigger) matched = true;
            else if ((filter.stringVal == "decoration" || filter.stringVal == "deco") && obj->m_objectType == GameObjectType::Decoration) matched = true;
            else if (filter.stringVal == "passable" && obj->m_isPassable) matched = true;
            else if (filter.stringVal == "detail" && obj->m_isHighDetail) matched = true;
            else if (filter.stringVal == "hazard" && obj->m_objectType == GameObjectType::Hazard) matched = true;
            else if (filter.stringVal == "solid" && obj->m_objectType == GameObjectType::Solid) matched = true;
            if (!validFilter(filter, matched)) return false;
        }
    }
    return true;
}


class $modify(EditUI, EditorUI) {
    bool init(LevelEditorLayer* lel) {	
        if (!EditorUI::init(lel)) return false;
        
        auto button = CCMenuItemSpriteExtra::create(
            CCSprite::create("popupbutton.png"_spr), 
            this, 
            menu_selector(EditUI::onOpenFilterMenu)
        );
        button->setPosition(ccp(162.0f, -21.5f));
        button->setScale(0.85f);
        button->m_baseScale = 0.85f;
        m_deleteMenu->addChild(button);
        
        return true;
    }

    void onOpenFilterMenu(CCObject* sender) {
        MoreSelectFiltersPopup::create()->show();
    }

    void selectObject(GameObject* p0, bool p1) {
        if (filtersEnabled) {
            if (validObject(p0, currentFilter)) {
                EditorUI::selectObject(p0, p1);
            }
        } else {
            EditorUI::selectObject(p0, p1);
        }
    }

    void selectObjects(CCArray* p0, bool p1) {
        if (filtersEnabled) {
            auto validObjectsArray = CCArray::create();
            auto filter = currentFilter;
            for (auto obj : CCArrayExt<GameObject*>(p0)) {
                if (validObject(obj, filter)) {
                    validObjectsArray->addObject(obj);
                }
            }
            if (validObjectsArray->count() > 0) {
                EditorUI::selectObjects(validObjectsArray, p1);
            }
        } else {
            EditorUI::selectObjects(p0, p1);
        }
    }
};

class $modify(Editor, LevelEditorLayer) {
    void removeObject(GameObject* p0, bool p1) {
        if (filtersEnabled && !p1) {
            if (validObject(p0, currentFilter)) {
                LevelEditorLayer::removeObject(p0, p1);
            }
        } else {
            LevelEditorLayer::removeObject(p0, p1);
        }
    }
};
