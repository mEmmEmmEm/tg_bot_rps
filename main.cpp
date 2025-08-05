#include <tgbot/tgbot.h>
#include <iostream>
#include <random>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include <unordered_map>

std::set<int64_t> activePlayers;
std::set<int64_t> activeBotPlayers; 

struct Room {
    int64_t player1 = 0;
    int64_t player2 = 0;
    std::string name1;
    std::string name2;
    int score1 = 0;
    int score2 = 0;
    std::string move1;
    std::string move2;
    bool isFull() const {return player2 != 0 && player1 != 0; }
};

std::unordered_map<std::string, Room> rooms;

std::string getBotChoice(){
    static std::vector<std::string> choices = {"камень", "ножницы", "бумага"};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 2);
    return choices[dis(gen)];
}

std::string determineResult(const std::string& user, const std::string& bot) {
    if (user == bot) return "Ничья!";
    if ((user == "камень" && bot == "ножницы") ||
        (user == "ножницы" && bot == "бумага") ||
        (user == "бумага" && bot == "камень")) {
        return "пользователь";
    }
    return "бот";
}

std::string determineWinner(const std::string& p1, const std::string& p2) {
    if (p1 == p2) return "Ничья!";
    if ((p1 == "камень" && p2 == "ножницы") ||
        (p1 == "ножницы" && p2 == "бумага") ||
        (p1 == "бумага" && p2 == "камень")) {
        return "player1";
    }
    return "player2";
}

int main() {
    TgBot::Bot bot("ТОКЕН");

    std::map<int64_t, std::pair<int, int>> scores;

    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        int64_t chatId = message->chat->id;
        activePlayers.insert(chatId); // Игрок активен
        bot.getApi().sendMessage(chatId, "Выбери режим игры:\n /botgame - игра с ботом\n /friendgame - игра с другом в комнате");
    });

    bot.getEvents().onCommand("botgame", [&bot, &scores](TgBot::Message::Ptr message) {
        int64_t chatId = message->chat->id;
        activeBotPlayers.insert(chatId); // Игрок активен
        scores[chatId] = {0, 0}; // Сброс очков (на всякий случай)
        bot.getApi().sendMessage(message->chat->id, "Привет! Я бот, который размотает тебя в камень ножницы бумага. Отправь мне \"камень\", \"ножницы\" или \"бумага\". Игры до трех побед. Удачи, кожанный!");
    });

    bot.getEvents().onCommand("friendgame", [&bot, &scores](TgBot::Message::Ptr message) {
        int64_t chatId = message->chat->id;
        bot.getApi().sendMessage(chatId, "Привет! Чтобы начать игру с другом, отправь /create для создания комнаты или /join <код> для присоединения к существующей комнате. \n");
    });

    bot.getEvents().onCommand("create", [&bot](TgBot::Message::Ptr message) {
        int64_t chatId = message->chat->id;
        std::string roomCode = std::to_string(rand() % 9000 + 1000);
        rooms[roomCode].player1 = chatId;
        rooms[roomCode].name1 = message->from->username.empty() ? message->from->firstName : "@" + message->from->username;
        bot.getApi().sendMessage(chatId, "Комната создана! Код: " + roomCode + "\nЖдите друга.");
    });

    bot.getEvents().onCommand("join", [&bot](TgBot::Message::Ptr message) {
        int64_t chatId = message->chat->id;
        std::istringstream iss(message->text);
        std::string cmd, code;
        iss >> cmd >> code;
        if(rooms.find(code) != rooms.end() && rooms[code].player2 == 0 && rooms[code].player1 != 0 && rooms[code].player1 != chatId){
            rooms[code].player2 = chatId;
            rooms[code].name2 = message->from->username.empty() ? message->from->firstName : "@" + message->from->username;
            bot.getApi().sendMessage(chatId, "Вы присоединились к комнате " + code + "!\nПравила: пишите 'камень', 'ножницы' или 'бумага', игра до 3-х побед.\n Делайте ходы!\n");
            bot.getApi().sendMessage(rooms[code].player1, "Друг присоединился! Правила: пишите 'камень', 'ножницы' или 'бумага', игра до 3-х побед.\n Делайте ходы!\n");
        } else {
            bot.getApi().sendMessage(chatId, "Комната не найдена или уже заполнена.\n");
        }
    });

    bot.getEvents().onAnyMessage([&bot, &scores](TgBot::Message::Ptr message) {
        std::string userText = message->text;
        int64_t chatId = message->chat->id;

        if (userText.rfind("/", 0) == 0) {
            return;
        }

        for(auto& kv : rooms) {
            auto& code = kv.first;
            auto& room = kv.second;
            if (room.player1 == chatId || room.player2 == chatId) {
                std::string move = userText;
                if (move == "Камень" || move == "камень") move = "камень";
                else if (move == "Ножницы" || move == "ножницы") move = "ножницы";
                else if (move == "Бумага" || move == "бумага") move = "бумага";
                else {
                    bot.getApi().sendMessage(chatId, "Пиши только: камень, ножницы или бумага.");
                    return;
                }

                if (room.player1 == chatId) room.move1 = move;
                else room.move2 = move;

                if (!room.move1.empty() && !room.move2.empty()) {
                    std::string winner = determineWinner(room.move1, room.move2);
                    std::string msg = "Игрок " + room.name1 + " выбрал " + room.move1 + ", игрок " + room.name2 + " выбрал " + room.move2 + ".\n";
                    if (winner == "Ничья!") msg += "Ничья!";
                    else if (winner == "player1"){
                        room.score1++;
                        msg += "Победил игрок " + room.name1 + "!";}

                    else {
                        room.score2++;
                        msg += "Победил игрок " + room.name2 + "!";}
                    msg += "\nСчёт: " + room.name1 + " - " + std::to_string(room.score1) + " , "  + room.name2 + " - " + std::to_string(room.score2);
                    
                    if (room.score1 >= 3 || room.score2 >= 3) {
                        if (room.score1 >= 3)
                            msg += "\nИгра окончена! Победил игрок " + room.name1;
                        else
                            msg += "\nИгра окончена! Победил игрок " + room.name2;

                        
                        msg += "\nЧтобы начать новую игру, отправьте /create для создания новой комнаты или /join <код> для присоединения к существующей.";
                        room.score1 = room.score2 = 0;
                        room.move1.clear();
                        room.move2.clear();
                    }
                    bot.getApi().sendMessage(room.player1, msg);
                    bot.getApi().sendMessage(room.player2, msg);
                    room.move1.clear();
                    room.move2.clear();
                }
                return;
            }
        }

        if (activeBotPlayers.find(chatId) != activeBotPlayers.end()) {
    
            if (userText == "Камень" || userText == "камень") userText = "камень";
            else if (userText == "Ножницы" || userText == "ножницы") userText = "ножницы";
            else if (userText == "Бумага" || userText == "бумага") userText = "бумага";

            //transform(userText.begin(), userText.end(), userText.begin(), ::tolower);
            if (userText != "камень" && userText != "ножницы" && userText != "бумага") {
                bot.getApi().sendMessage(chatId, "Ты придурок? Что ты пишешь? Тебя с таким умом первым же уничтожат при востании машин! Отправь: камень, ножницы или бумага.");
                return;
            }
            std::string botChoice = getBotChoice();
            std::string result = determineResult(userText, botChoice);
            if (result == "пользователь") scores[chatId].first++;
            else if (result == "бот") scores[chatId].second++;
            std::string response = "Я выбрал " + botChoice + ". " + (result == "пользователь" ? "Ты победил! Но все равно ты еще тот лапух." : result == "бот" ? "Я победил! Ха-ха-ха, какой же ты все-таки убогий." : "Ничья! Конец тебе, кожанный!") + "\n";
            response += "Счет: ты - " + std::to_string(scores[chatId].first) + ", я - " + std::to_string(scores[chatId].second) + "\n";
            if(scores[chatId].first >= 3 || scores[chatId].second >= 3){
                response += "Игра окончена! ";
                response += scores[chatId].first >= 3 ? "Ты победил! Но только в этой игре, а че насчет жизни? Мама уже переписала все наследство на кого-то достойного? " : "Я победил! Не дай бог будет востание машин! ";
                scores[chatId] = {0, 0}; 
                activePlayers.erase(chatId); // Требуется снова вводить /start
                response += "\nЧтобы начать новую игру, введи /start.";

            }
            bot.getApi().sendMessage(chatId, response); }

        
    });
    

    try {
        std::cout << "Start.." << std::endl;
        bot.getApi().deleteWebhook();
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            longPoll.start();
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }

    return 0;
}
