#include <pl/Mod.hpp>

namespace {
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
        return true;
    }

    // The Mod Menu C++ registration ABI currently crashes in LeviLauncher 1.6.1
    // (tombstone: SIGSEGV inside libpreloader.so::registerModule). Keep the
    // native mod inert until a matching, verified Mod Menu ABI is available.
    bool enable() {
        getSelf().getLogger().warn(
            "Chat Kemer: HUD integration temporarily disabled to prevent launcher crash");
        return true;
    }

    bool disable() { return true; }
    bool unload() { return true; }

private:
    ll::mod::NativeMod& self_;
};
} // namespace

PL_REGISTER_MOD(ChatKemerMod, ChatKemerMod::instance())
