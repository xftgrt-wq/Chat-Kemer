#include <pl/Mod.hpp>
#include <pl/ModMenu.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <string_view>

namespace {
constexpr std::string_view kModuleId = "chat_kemer.main";
constexpr std::array<std::string_view, 6> kDefaultCommands{
    "/spawn", "/top", "/home", "/warp", "/kit", "/shop"};
constexpr std::string_view kButtonScaleKey = "buttonScale";
constexpr std::string_view kOpacityKey = "opacity";
constexpr std::uint32_t kHudSnap = pl::modmenu::HudSnapGrid |
                                   pl::modmenu::HudSnapElements |
                                   pl::modmenu::HudSnapScreenCenter;

class ChatKemerMod {
public:
    static ChatKemerMod& instance() {
        static ChatKemerMod mod;
        return mod;
    }

    ChatKemerMod() : self_(*ll::mod::NativeMod::current()) {
        for (std::size_t i = 0; i < commands_.size(); ++i) {
            commands_[i] = kDefaultCommands[i];
        }
    }

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return self_; }

    bool load() {
        getSelf().getLogger().info("Chat Kemer: loaded");
        return true;
    }

    bool enable() {
        auto& logger = getSelf().getLogger();
        pl::modmenu::ModuleInfo info;
        info.moduleId = std::string(kModuleId);
        info.displayName = "Chat Kemer";
        info.description = "Six quick-command buttons for Minecraft Bedrock";
        info.modId = getSelf().getId();
        info.defaultEnabled = true;

        for (std::size_t i = 0; i < commands_.size(); ++i) {
            info.configs.push_back({
                "command" + std::to_string(i + 1),
                "Команда " + std::to_string(i + 1),
                pl::modmenu::ConfigType::Text,
                commands_[i], {}, {}, {}});
        }
        info.configs.push_back({
            std::string(kButtonScaleKey), "Размер кнопок",
            pl::modmenu::ConfigType::SliderFloat, "1.0", "0.6", "1.8", {}});
        info.configs.push_back({
            std::string(kOpacityKey), "Прозрачность кнопок",
            pl::modmenu::ConfigType::SliderInt, "85", "20", "100", {}});

        info.onConfigChanged = [this](std::string_view,
                                      std::string_view key,
                                      std::string_view value) {
            if (key.size() == 8 && key.substr(0, 7) == "command") {
                const char digit = key.back();
                if (digit >= '1' && digit <= '6') {
                    commands_[static_cast<std::size_t>(digit - '1')] =
                        std::string(value);
                }
                return;
            }
            if (key == kButtonScaleKey) {
                char* end = nullptr;
                const std::string raw(value);
                const float parsed = std::strtof(raw.c_str(), &end);
                if (end != raw.c_str() && *end == '\0') {
                    buttonScale_ = parsed < 0.6f ? 0.6f
                                  : parsed > 1.8f ? 1.8f : parsed;
                    refreshButtons();
                }
                return;
            }
            if (key == kOpacityKey) {
                char* end = nullptr;
                const std::string raw(value);
                const long parsed = std::strtol(raw.c_str(), &end, 10);
                if (end != raw.c_str() && *end == '\0') {
                    opacity_ = static_cast<int>(parsed < 20 ? 20 : parsed > 100 ? 100 : parsed);
                    refreshButtons();
                }
            }
        };

        if (!pl::modmenu::registerModule(info)) {
            logger.error("Chat Kemer: module registration failed");
            return false;
        }
        if (!registerButtons()) {
            pl::modmenu::unregisterModule(kModuleId);
            return false;
        }
        registerHudEditorElements();
        logger.info("Chat Kemer: six HUD buttons and HUD Editor entries registered");
        return true;
    }

    bool disable() {
        for (std::size_t i = 0; i < commands_.size(); ++i) {
            pl::modmenu::unregisterButton(buttonId(i));
        }
        pl::modmenu::submitHudEditorElements(kModuleId, {});
        pl::modmenu::unregisterModule(kModuleId);
        return true;
    }

    bool unload() { return true; }

private:
    ll::mod::NativeMod& self_;
    std::array<std::string, 6> commands_{};
    float buttonScale_{1.0f};
    int opacity_{85};

    static std::string buttonId(std::size_t index) {
        return "chat_kemer.button." + std::to_string(index + 1);
    }

    bool registerButtons() {
        for (std::size_t i = 0; i < commands_.size(); ++i) {
            const std::string label = std::to_string(i + 1);
            const bool ok = pl::modmenu::ButtonBuilder(
                                buttonId(i), "Chat Kemer " + label)
                .modId(getSelf().getId())
                .moduleId(std::string(kModuleId))
                .label(label)
                .styleColors(
                    (static_cast<std::uint32_t>(opacity_ * 255 / 100) << 24) | 0x0024282Cu,
                    (static_cast<std::uint32_t>(opacity_ * 255 / 100) << 24) | 0x004AE0A0u,
                    0xFF4AE0A0u)
                .textColor(0xFFFFFFFFu)
                .behavior(pl::modmenu::ButtonBehavior::Click)
                .defaultVisible(true)
                .sizeScale(buttonScale_, buttonScale_)
                .onEvent([this, i](std::string_view,
                                   pl::modmenu::ButtonEvent event, float) {
                    if (event != pl::modmenu::ButtonEvent::Click) return;
                    getSelf().getLogger().warn(
                        "Chat Kemer button {} pressed; configured command is '{}'. "
                        "Chat dispatch is not connected yet.",
                        i + 1, commands_[i]);
                })
                .registerButton();

            if (!ok) {
                getSelf().getLogger().error(
                    "Chat Kemer: could not register button {}; rolling back", i + 1);
                for (std::size_t registered = 0; registered < i; ++registered) {
                    pl::modmenu::unregisterButton(buttonId(registered));
                }
                return false;
            }
        }
        return true;
    }

    void registerHudEditorElements() {
        std::array<pl::modmenu::HudEditorElement, 6> elements{};
        for (std::size_t i = 0; i < elements.size(); ++i) {
            auto& element = elements[i];
            element.elementId = buttonId(i);
            element.displayName = "Chat Kemer — кнопка " + std::to_string(i + 1);
            element.positionKeyX = "button" + std::to_string(i + 1) + "X";
            element.positionKeyY = "button" + std::to_string(i + 1) + "Y";
            element.snapGroup = "chat-kemer-buttons";
            element.x = 24.0f + static_cast<float>(i % 3) * 60.0f;
            element.y = 120.0f + static_cast<float>(i / 3) * 60.0f;
            element.width = 52.0f * buttonScale_;
            element.height = 52.0f * buttonScale_;
            element.gridSize = 8.0f;
            element.snapThreshold = 12.0f;
            element.gridGap = 4.0f;
            element.snapFlags = kHudSnap;
        }
        pl::modmenu::submitHudEditorElements(kModuleId, elements);
    }

    void refreshButtons() {
        for (std::size_t i = 0; i < commands_.size(); ++i) {
            pl::modmenu::unregisterButton(buttonId(i));
        }
        if (registerButtons()) {
            registerHudEditorElements();
        } else {
            getSelf().getLogger().error(
                "Chat Kemer: button refresh failed; some buttons may be unavailable");
        }
    }
};
} // namespace

PL_REGISTER_MOD(ChatKemerMod, ChatKemerMod::instance())
