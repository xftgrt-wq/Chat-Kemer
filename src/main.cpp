#include <pl/Mod.hpp>
#include <pl/ModMenu.hpp>

#include <array>
#include <string>
#include <string_view>

namespace {
constexpr std::string_view kModId = "chat-kemer";
constexpr std::string_view kModuleId = "chat-kemer.main";

class ChatKemerMod {
public:
    static ChatKemerMod& instance() {
        static ChatKemerMod mod;
        return mod;
    }

    ChatKemerMod() : self_(*ll::mod::NativeMod::current()) {}

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return self_; }

    bool load() {
        getSelf().getLogger().info("Chat Kemer: loaded");

        pl::modmenu::ModuleBuilder module(
            std::string(kModuleId), "Chat Kemer");
        module.description("Six configurable-position HUD buttons")
            .modId(std::string(kModId))
            .defaultEnabled(true)
            .hideInHudEditor(false);

        if (!module.registerModule()) {
            getSelf().getLogger().error(
                "Chat Kemer: Mod Menu module registration failed");
            return false;
        }

        moduleRegistered_ = true;
        return true;
    }

    bool enable() {
        if (!moduleRegistered_) {
            getSelf().getLogger().error(
                "Chat Kemer: module was not registered");
            return false;
        }

        constexpr std::array<const char*, 6> labels{
            "Chat 1", "Chat 2", "Chat 3",
            "Chat 4", "Chat 5", "Chat 6"
        };

        bool allRegistered = true;
        for (std::size_t i = 0; i < labels.size(); ++i) {
            const std::string id = "chat-kemer.button." + std::to_string(i + 1);
            pl::modmenu::ButtonBuilder button(id, labels[i]);
            button.moduleId(std::string(kModuleId))
                .modId(std::string(kModId))
                .label(labels[i])
                .behavior(pl::modmenu::ButtonBehavior::Click)
                .defaultVisible(true)
                .stylePreset(pl::modmenu::ButtonStylePreset::Keycap)
                .styleColors(0xFF202020u, 0xFFFFFFFFu, 0xFFFFFFFFu)
                .textColor(0xFFFFFFFFu)
                .activeTextColor(0xFF111111u)
                .sizeScale(1.0f, 1.0f)
                .onEvent([this, i](std::string_view, pl::modmenu::ButtonEvent event,
                                   float) {
                    if (event == pl::modmenu::ButtonEvent::Click) {
                        getSelf().getLogger().info(
                            "Chat Kemer: HUD button {} clicked", i + 1);
                    }
                });

            if (!button.registerButton()) {
                allRegistered = false;
                getSelf().getLogger().error(
                    "Chat Kemer: failed to register HUD button {}", i + 1);
            }
        }

        buttonsRegistered_ = allRegistered;
        if (allRegistered) {
            getSelf().getLogger().info(
                "Chat Kemer: six black/white HUD buttons registered");
        }
        return allRegistered;
    }

    bool disable() {
        for (int i = 1; i <= 6; ++i) {
            pl::modmenu::unregisterButton(
                "chat-kemer.button." + std::to_string(i));
        }
        buttonsRegistered_ = false;
        return true;
    }

    bool unload() {
        disable();
        if (moduleRegistered_) {
            pl::modmenu::unregisterModule(kModuleId);
            moduleRegistered_ = false;
        }
        return true;
    }

private:
    ll::mod::NativeMod& self_;
    bool moduleRegistered_{false};
    bool buttonsRegistered_{false};
};
} // namespace

PL_REGISTER_MOD(ChatKemerMod, ChatKemerMod::instance())
