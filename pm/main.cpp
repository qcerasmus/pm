#include <memory>
#include <string>

#include "clip.h"
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

using namespace ftxui;

struct bla
{
    std::string label;
    std::string username;
    std::string password;
};

int main()
{
    std::vector<bla> passwords;
    passwords.push_back({"reddit", "asdfasdfasdf", "abskdlfjkdsjdklf"});
    passwords.push_back({"facebook", "asdfasdfasdf", "abskdlfjkdsjdklf"});
    passwords.push_back({"youtube", "asdfasdfasdf", "abskdlfjkdsjdklf"});

    std::vector<Component> menu_items;

    for (const auto &password : passwords)
    {
        menu_items.push_back(MenuEntry(password.label));
    }

    auto status_box = text("press 'u' to copy the username and 'p' to copy the password");

    int selected = 0;
    auto menu = Container::Vertical(menu_items, &selected);
    menu |= CatchEvent(
        [&](Event event)
        {
            if (event.character() == "u")
            {
                status_box = text("copied username for " + passwords[selected].label + " to clipboard");
                clip::set_text(passwords[selected].username);
                return true;
            }
            else if (event.character() == "p")
            {
                status_box = text("copied password for " + passwords[selected].label + " to clipboard");
                clip::set_text(passwords[selected].password);
                return true;
            }

            return false;
        });

    auto component = Renderer(
        menu,
        [&]
        { return vbox({
                     text("selected = " + passwords[selected].label) | center,
                     separator(),
                     menu->Render() | flex,
                     separator(),
                     status_box | center,
                 }) |
                 border; });

    auto screen = ScreenInteractive::TerminalOutput();
    screen.Loop(component);
    return 0;
}
