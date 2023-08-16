#include <example/base64.h>
#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <xxtea.h>

#include "clip.h"
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

using namespace ftxui;

struct login_struct
{
    std::string label;
    std::string username;
    std::string password;
};

std::string Encrypt(const std::string &text, const std::string &key)
{
    size_t len;
    unsigned char *encrypted_data = (unsigned char *)xxtea_encrypt(text.c_str(), text.length(), key.c_str(), &len);
    const char *enc = base64_encode(encrypted_data, len);

    return std::string(enc);
}

std::string Decrypt(const std::string &encrypted_data, const std::string &key)
{
    size_t len;
    const auto *bla = base64_decode(encrypted_data.c_str(), &len);
    char *decrypted = (char *)xxtea_decrypt(bla, len, key.c_str(), &len);

    return std::string(decrypted, len);
}

Component Wrap(std::string name, Component component)
{
    return Renderer(
        component,
        [name, component]
        { return hbox({
                     text(name) | size(WIDTH, EQUAL, 8),
                     separator(),
                     component->Render() | xflex,
                 }) |
                 xflex; });
}

std::vector<login_struct> passwords;
void SavePasswords()
{
    std::ofstream out_file(".pwdb", std::ios_base::out);
    for (const auto &password : passwords)
    {
        out_file << password.label << "~" << password.username << "~" << password.password << "\n";
    }
    out_file.close();
}

void ReadPasswords()
{
    std::ifstream in_file(".pwdb");
    std::string line;
    while (std::getline(in_file, line))
    {
        std::string sub_strings[3];
        int tilde_counter = 0;
        for (const auto &ch : line)
        {
            if (ch == '~')
            {
                tilde_counter++;
                continue;
            }
            sub_strings[tilde_counter] += ch;
        }
        passwords.push_back({sub_strings[0], sub_strings[1], sub_strings[2]});
    }
}

int main(int argc, char *argv[])
{
    std::cout << "Please enter your passphrase: \n";
    std::string pass_phrase;
    std::cin >> pass_phrase;
    auto screen = ScreenInteractive::Fullscreen();

    auto status_box = text("Status: ");
    auto shortcut_box = text("Shortcuts: 'u' - copy username, 'p' - copy password, 'd' - delete entry.");

    ReadPasswords();

    std::vector<std::string> menu_items;
    for (const auto &password : passwords)
    {
        menu_items.push_back(password.label);
    }
    auto menu_selected = 0;
    auto menu = Menu(&menu_items, &menu_selected);

    menu |= CatchEvent(
        [&](Event event)
        {
            if (event.character() == "u")
            {
                status_box = text("Status: copied username for " + passwords[menu_selected].label + " to clipboard");
                clip::set_text(passwords[menu_selected].username);
                return true;
            }
            else if (event.character() == "p")
            {
                status_box = text("Status: copied password for " + passwords[menu_selected].label + " to clipboard");
                clip::set_text(Decrypt(passwords[menu_selected].password, pass_phrase));
                return true;
            }
            else if (event.character() == "d")
            {
                status_box = text("Status: deleted entry: " + passwords[menu_selected].label);
                menu_items.erase(menu_items.begin() + menu_selected);
                passwords.erase(passwords.begin() + menu_selected);
            }

            return false;
        });
    menu = Wrap("Logins", menu);

    std::string new_label, new_username, new_password;
    auto label_input = Input(&new_label, "label");
    label_input = Wrap("New Label", label_input);

    auto username_input = Input(&new_username, "username");
    username_input = Wrap("New Username", username_input);

    auto password_input = Input(&new_password, "password");
    password_input = Wrap("New Password", password_input);

    std::string button_label = "Add";
    auto add_button = Button(
        &button_label,
        [&]()
        {
            if (new_label.empty() || new_username.empty())
                return;
            passwords.push_back({new_label, new_username, Encrypt(new_password, pass_phrase)});
            menu_items.push_back(new_label);
            new_label = "";
            new_username = "";
            new_password = "";
            SavePasswords();
            label_input->TakeFocus();
        });
    add_button = Wrap("Add new password", add_button);

    std::string save_button_label = "Save";
    auto save_button = Button(
        &save_button_label,
        [&]()
        {
            status_box = text("Status: Saving passwords now: " + passwords[menu_selected].label);
            SavePasswords();
            status_box = text("Status: Done saving passwords now: " + passwords[menu_selected].label);
        });

    auto layout = Container::Vertical(
        {menu,
         label_input,
         username_input,
         password_input,
         add_button,
         save_button});

    auto component = Renderer(layout, [&]
                              { return vbox(
                                           {menu->Render() | flex,
                                            separator(),
                                            label_input->Render(),
                                            username_input->Render(),
                                            password_input->Render(),
                                            add_button->Render(),
                                            save_button->Render(),
                                            separator(),
                                            status_box,
                                            separator(),
                                            shortcut_box}) |
                                       xflex | border; });
    screen.Loop(component);
    return 0;
}
