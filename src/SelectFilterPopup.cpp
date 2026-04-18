#include <Geode/Geode.hpp>
#include "SelectFilterPopup.hpp"
#include "ParseInput.hpp"

using namespace geode::prelude;

std::vector<Filter> currentFilter;
bool filtersEnabled = true;

MoreSelectFiltersPopup* MoreSelectFiltersPopup::create() { 
    auto ret = new MoreSelectFiltersPopup();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool MoreSelectFiltersPopup::init() {
    if (!Popup::init(350.0f, 276.0f)) {
        return false;
    }

    this->setup();

    return true;
}


bool MoreSelectFiltersPopup::setup() {
    this->setTitle("Select Filter");

    auto mainMenu = CCMenu::create();
    mainMenu->setContentSize(m_size);
    mainMenu->setPosition(0.0f, 0.0f);
    m_mainLayer->addChild(mainMenu);

    auto okButton = CCMenuItemSpriteExtra::create(ButtonSprite::create("Ok"), this, menu_selector(MoreSelectFiltersPopup::onSaveValuesAndExit));
    okButton->setPosition(175.0f, 25.0f);
    mainMenu->addChild(okButton);

    auto clearButton = CCMenuItemSpriteExtra::create(CircleButtonSprite::create(CCSprite::create("clear.png"_spr), CircleBaseColor::Pink), this, menu_selector(MoreSelectFiltersPopup::onClearAll));
    clearButton->setPosition(25.0f, 25.0f);
    clearButton->setScale(0.75f);
    clearButton->m_baseScale = 0.75f;
    mainMenu->addChild(clearButton);

    auto selectAllButton = CCMenuItemSpriteExtra::create(CircleButtonSprite::create(CCSprite::create("selectall.png"_spr), CircleBaseColor::DarkAqua), this, menu_selector(MoreSelectFiltersPopup::onSelectAll));
    selectAllButton->setPosition(325.0f, 25.0f);
    selectAllButton->setScale(0.75f);
    selectAllButton->m_baseScale = 0.75f;
    mainMenu->addChild(selectAllButton);

    auto disableFiltersToggle = CCMenuItemToggler::create(CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
    CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"), this, menu_selector(MoreSelectFiltersPopup::onToggleFilters));
    disableFiltersToggle->toggle(!filtersEnabled);
    disableFiltersToggle->setPosition(290.0f, 25.0f);
    disableFiltersToggle->setScale(0.75f);
    mainMenu->addChild(disableFiltersToggle);

    auto filtersContainer = CCNode::create();
    filtersContainer->setPosition(0.0f, 0.0f);
    m_filtersContainer = filtersContainer;
    m_mainLayer->addChild(filtersContainer);

    auto createFilterCell = [this, filtersContainer] (const std::string& type, CCPoint pos) {
        auto menu = CCMenu::create();
        menu->setContentSize(CCSize(150.0f, 46.0f));
        menu->setPosition(pos);
        menu->setID(type);
        filtersContainer->addChild(menu);
        auto label = CCLabelBMFont::create(type.c_str(), "bigFont.fnt");
        label->setPosition(45.0f, 10.0f);
        label->setScale(0.5f);
        menu->addChild(label);
        auto input = TextInput::create(120.0f, "");
        input->setPosition(45.0f, 32.0f);
        input->setScale(0.75f);
        menu->addChild(input);
        input->setFilter("1234567890abcdefghijklmnopqrstuvwxyz.,-");
        auto clearButton = CCMenuItemSpriteExtra::create(CircleButtonSprite::create(CCSprite::create("clear.png"_spr),
        CircleBaseColor::Gray), this, menu_selector(MoreSelectFiltersPopup::onClear));
        clearButton->setPosition(106.0f, 11.0f);
        clearButton->setScale(0.5f);
        clearButton->m_baseScale = 0.5f;
        menu->addChild(clearButton);
        auto copyFromObjectButton = CCMenuItemSpriteExtra::create(CircleButtonSprite::create(CCSprite::create("copyfromobject.png"_spr)), this, menu_selector(MoreSelectFiltersPopup::onCopyFromObject));
        copyFromObjectButton->setPosition(135.0f, 23.0f);
        copyFromObjectButton->setScale(0.75f);
        copyFromObjectButton->m_baseScale = 0.75f;
        menu->addChild(copyFromObjectButton);
        auto equalsToggler = CCMenuItemToggler::create(CircleButtonSprite::create(CCSprite::create("equals.png"_spr), CircleBaseColor::Cyan),
        CircleButtonSprite::create(CCSprite::create("notequals.png"_spr), CircleBaseColor::DarkPurple), this, nullptr);
        equalsToggler->setPosition(106.0f, 35.0f);
        equalsToggler->setScale(0.5f);
        menu->addChild(equalsToggler);
    };

    createFilterCell("object id", ccp(20.0f, 45.0f));
    createFilterCell("group", ccp(20.0f, 95.0f));
    createFilterCell("layer", ccp(20.0f, 145.0f));
    createFilterCell("scale", ccp(20.0f, 195.0f));
    createFilterCell("z order", ccp(180.0f, 45.0f));
    createFilterCell("color", ccp(180.0f, 95.0f));
    createFilterCell("hsv", ccp(180.0f, 145.0f));
    createFilterCell("type", ccp(180.0f, 195.0f));

    for (auto& filter : currentFilter) {
        std::string string = "";
        if (filter.type == "object id" || filter.type == "group" || filter.type == "z order") string = std::to_string(filter.intVal);
        else if (filter.type == "layer" || filter.type == "color") {
            if (intTypesMapReverse.contains(filter.intVal)) string = intTypesMapReverse.at(filter.intVal);
            else string = std::to_string(filter.intVal);
            if (filter.type == "color" && filter.intVal > 0 && filter.intVal < 1000) string = std::to_string(filter.intVal);
        }
        else if (filter.type == "scale" || filter.type == "hsv") {
            if (filter.typedFloatVal.type == 1) string = floatToFormattedString(filter.typedFloatVal.value) + (filter.type == "hsv" ? "s" : "x");
            else if (filter.typedFloatVal.type == 2) string = floatToFormattedString(filter.typedFloatVal.value) + (filter.type == "hsv" ? "b" : "y");
            else string = floatToFormattedString(filter.typedFloatVal.value) + (filter.type == "hsv" ? "h" : "");
        }
        else string = filter.stringVal;
        auto input = getTextInput(filter.type);
        std::string currentStr = input->getString().c_str();
        input->setString( (currentStr + string + ",").c_str() );
        getToggler(filter.type)->toggle(filter.notEquals);
    }
    for (auto menu : CCArrayExt<CCNode*>(filtersContainer->getChildren())) trimTextInput(menu->getID());

    return true;
}

void MoreSelectFiltersPopup::onClear(CCObject* sender) {
    getTextInput(sender)->setString("");
}

void MoreSelectFiltersPopup::onCopyFromObject(CCObject* sender) {
    auto objs = EditorUI::get()->getSelectedObjects();
    auto filter = getFilterType(sender);
    auto input = getTextInput(sender);
    if (objs->count() == 0) {
        FLAlertLayer::create("Select Filter", "No objects selected :3", "Ok")->show();
        return;
    }
    else if (objs->count() > 1 && filter == "type") {
        FLAlertLayer::create("Select Filter", "Multiple objects not supported for this filter :3", "Ok")->show();
        return;
    }
    for (auto obj : CCArrayExt<GameObject*>(objs)) {
        std::string currentStr = input->getString().c_str();
        
        if (filter == "object id") input->setString( (currentStr + "," + std::to_string(obj->m_objectID)).c_str() );
        else if (filter == "group") {
            if (auto groups = obj->m_groups) {
                for (auto group : *groups) {
                    if (group != 0) {
                        currentStr = input->getString().c_str();
                        input->setString( (currentStr + "," + std::to_string(group)).c_str() );
                    }
                }
            }
        }
        else if (filter == "layer") {
            auto layer = static_cast<int>(obj->m_zLayer) == 0 ? static_cast<int>(obj->m_defaultZLayer) : static_cast<int>(obj->m_zLayer);
            input->setString( (currentStr + "," + intTypesMapReverse.at(layer)).c_str() );
        }
        else if (filter == "z order") {
            auto order = obj->m_zOrder == 0 ? obj->m_defaultZOrder : obj->m_zOrder;
            input->setString( (currentStr + "," + std::to_string(order)).c_str() );
        }
        else if (filter == "color") {
            std::vector<GJSpriteColor*> cols;
            for (auto col : {obj->m_baseColor, obj->m_detailColor}) if (col) cols.push_back(col);
            for (auto col : cols) {
                auto color = col->m_colorID == 0 ? col->m_defaultColorID : col->m_colorID;
                currentStr = input->getString().c_str();
                std::string val;
                if (intTypesMapReverse.contains(color)) val = intTypesMapReverse.at(color);
                else val = std::to_string(color);
                input->setString( (currentStr + "," + val).c_str() );
            }
        }
        else if (filter == "scale") {
            std::string string = "";
            if (obj->m_scaleX == obj->m_scaleY) string = floatToFormattedString(obj->m_scaleX);
            else string = floatToFormattedString(obj->m_scaleX) + "x," + floatToFormattedString(obj->m_scaleY) + "y";
            input->setString( (currentStr + "," + string).c_str() );
        }
        else if (filter == "hsv") {
            std::vector<GJSpriteColor*> cols;
            for (auto col : {obj->m_baseColor, obj->m_detailColor}) if (col) cols.push_back(col);
            for (auto col : cols) {
                if (!col->m_usesHSV) continue;
                currentStr = input->getString().c_str();
                if (col->m_hsv.h != 0) input->setString( (currentStr + "," + floatToFormattedString(col->m_hsv.h) + "h").c_str() );
                currentStr = input->getString().c_str();
                if (col->m_hsv.s != 1) input->setString( (currentStr + "," + floatToFormattedString(col->m_hsv.s) + "s").c_str() );
                currentStr = input->getString().c_str();
                if (col->m_hsv.v != 1) input->setString( (currentStr + "," + floatToFormattedString(col->m_hsv.v) + "b").c_str() );
            }
        }
        else if (filter == "type") {
            std::string string = "";
            if (!(obj->m_objectType == GameObjectType::Decoration)) string += ",layout";
            if (obj->m_isTrigger) string += ",trigger";
            if (obj->m_objectType == GameObjectType::Decoration) string += ",decoration";
            if (obj->m_isPassable) string += ",passable";
            if (obj->m_isHighDetail) string += ",detail";
            if (obj->m_objectType == GameObjectType::Hazard) string += ",hazard";
            if (obj->m_objectType == GameObjectType::Solid) string += ",solid";
            input->setString( (currentStr + string).c_str() );
        }
        trimTextInput(filter);
        std::string finalStr = input->getString().c_str();
        input->setString( trimDuplicates(finalStr).c_str() );
    }
}

void MoreSelectFiltersPopup::onSaveValuesAndExit(CCObject* sender) {
    saveValues();
    this->onClose(nullptr);
}

void MoreSelectFiltersPopup::onClearAll(CCObject* sender) {
    for (auto menu : CCArrayExt<CCNode*>(m_filtersContainer->getChildren())) getTextInput(menu->getID())->setString("");
}

void MoreSelectFiltersPopup::onSelectAll(CCObject* sender) {
    auto editor = EditorUI::get();
    editor->selectObjects(LevelEditorLayer::get()->m_objects, true);
    editor->updateButtons();
}

void MoreSelectFiltersPopup::onToggleFilters(CCObject* sender) {
    filtersEnabled = !filtersEnabled;
}

void MoreSelectFiltersPopup::saveValues() {
    currentFilter.clear();
    std::vector<std::string> type {"object id", "group", "layer", "z order", "color", "scale", "hsv", "type"};
    for (int i = 0; i < 5; i++) {
        auto notEquals = getTogglerEquals(type[i]);
        std::string str = getTextInput(type[i])->getString().c_str();
        for (auto num : getIntVector(str)) currentFilter.emplace_back(Filter{type[i], 0, num, {}, "", notEquals});
    }
    for (int i = 0; i < 2; i++) {
        auto notEquals = getTogglerEquals(type[i + 5]);
        std::string str = getTextInput(type[i + 5])->getString().c_str();
        for (auto& typedFloat : getTypedFloatVector(str)) currentFilter.emplace_back(Filter{type[i + 5], 1, 0, typedFloat, "", notEquals});
    }
    auto notEquals = getTogglerEquals(type[7]);
    std::string str = getTextInput(type[7])->getString().c_str();
    for (auto& string : getStringVector(str)) currentFilter.emplace_back(Filter{type[7], 2, 0, {}, string, notEquals});
}

TextInput* MoreSelectFiltersPopup::getTextInput(const std::string& id) {
    return m_filtersContainer->getChildByID(id)->getChildByType<TextInput>(0);
}

TextInput* MoreSelectFiltersPopup::getTextInput(CCObject* button) {
    return static_cast<CCNode*>(button)->getParent()->getChildByType<TextInput>(0);
}

void MoreSelectFiltersPopup::trimTextInput(const std::string& id) {
    auto input = getTextInput(id);
    std::string string = input->getString().c_str();
    if (!string.empty()) {
        if (string[0] == ',') {
            string = string.substr(1);
        }
        if (string[string.size() - 1] == ',') {
            string = string.substr(0, string.size() - 1);
        }
    }
    input->setString(string.c_str());
}

std::string MoreSelectFiltersPopup::getFilterType(CCObject* button) {
    return static_cast<CCNode*>(button)->getParent()->getID();
}

bool MoreSelectFiltersPopup::getTogglerEquals(const std::string& id) {
    return m_filtersContainer->getChildByID(id)->getChildByType<CCMenuItemToggler>(0)->m_toggled;
}

CCMenuItemToggler* MoreSelectFiltersPopup::getToggler(const std::string& id) {
    return m_filtersContainer->getChildByID(id)->getChildByType<CCMenuItemToggler>(0);
}
