#include "queue.h"
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

class Song
{
public:
    string name;
    int duration;
    int preference;
    bool played = false;
};

class Playlist
{
private:
    Queue<Song> vip;
    Queue<Song> regular;
    Queue<Song> low;
    Queue<Song> playlist_history;
    Song current_song;
    bool isPlaying = false;
public:
    void add(Song song)
    {
        if (song.duration <= 0)
        {
            ofstream response("output.txt", ios::app);
            response << "Invalid duration." << endl;
            response.close();
            return;
        }
        if (song.preference < 1 || song.preference > 3)
        {
            ofstream response("output.txt", ios::app);
            response << "Invalid preference."<< endl;
            response.close();
            return;
        }
        song.name.erase(0, song.name.find_first_not_of(" \t\n\r"));
        if (song.name.empty())
        {
            ofstream response("output.txt", ios::app);
            response << "Name field is empty." << endl;
            response.close();
            return;
        }
        if (song.preference == 1)
        {
            vip.enqueue(song);
        }
        else if (song.preference == 2)
        {
            regular.enqueue(song);
        }
        else if (song.preference == 3)
        {
            low.enqueue(song);
        }
    }
    void play()
    {
        ofstream response("output.txt", ios::app);
        if (isPlaying)
        {
            response << "Song already playing: " << current_song.name << " (" << current_song.duration << "s)." << endl;
            response.close();
            return;
        }
        Song next_song;
        if (!vip.isEmpty())
        {
            next_song = vip.dequeue();
        }
        else if (!regular.isEmpty())
        {
            next_song = regular.dequeue();
        }
        else if (!low.isEmpty())
        {
            next_song = low.dequeue();
        }
        else
        {
            response << "Empty playlist." << endl;
            response.close();
            return;
        }
        current_song = next_song;
        isPlaying = true;
        response << "Now playing: " << current_song.name << " (" << current_song.duration << "s)." << endl;
        response.close();
    }
    void skip(int n)
    {
        ofstream response("output.txt", ios::app);
        if (!isPlaying)
        {
            response << "No song is playing right now." << endl;
            response.close();
            return;
        }
        if (n <= 0 )
        {
            response << "Invalid skipped seconds value." << endl;
            response.close();
            return;
        }
        if (n < current_song.duration)
        {
            current_song.duration -= n;
            response << "Skipped " << n << "s. " << current_song.duration << "s remaining." << endl;
        }
        else
        {
            response << "Skipped " << n << "s. Song finished." << endl;
            current_song.played = true;
            playlist_history.enqueue(current_song);
            isPlaying = false;
        }
        response.close();
    }
    void replay()
    {
        ofstream response("output.txt", ios::app);
        if (playlist_history.isEmpty())
        {
            response << "No history to replay." << endl;
            response.close();
            return;
        }
        Queue<Song> temp;
        Song last_played_song;
        while (!playlist_history.isEmpty())
        {
            last_played_song = playlist_history.dequeue();
            temp.enqueue(last_played_song);
        }
        while (!temp.isEmpty())
        {
            playlist_history.enqueue(temp.dequeue());
        }
        Queue<Song>* targetQueue = nullptr;
        if (last_played_song.preference == 1)
        {
            targetQueue = &vip;
        }
        else if (last_played_song.preference == 2)
        {
            targetQueue = &regular;
        }
        else if (last_played_song.preference == 3)
        {
            targetQueue = &low;
        }
        if (targetQueue != nullptr)
        {
            Queue<Song> tempQueue;
            while (!targetQueue->isEmpty())
            {
                tempQueue.enqueue(targetQueue->dequeue());
            }
            targetQueue->enqueue(last_played_song);
            while (!tempQueue.isEmpty())
            {
                targetQueue->enqueue(tempQueue.dequeue());
            }
        }
        response << "Replaying: " << last_played_song.name << endl;
        response.close();
    }
    void remove(string songName)
    {
        ofstream response("output.txt", ios::app);
        songName.erase(0, songName.find_first_not_of(" \t\n\r"));
        songName.erase(songName.find_last_not_of(" \t\n\r") + 1);
        if (songName.empty())
        {
            response << "Invalid song name." << endl;
            response.close();
            return;
        }
        bool remove_done = false;
        Queue<Song> temp_queue;
        while (!vip.isEmpty())
        {
            Song temp_song = vip.dequeue();
            if (!remove_done && temp_song.name == songName)
            {
                remove_done = true;
            }
            else
            {
                temp_queue.enqueue(temp_song);
            }
        }
        while (!temp_queue.isEmpty())
        {
            vip.enqueue(temp_queue.dequeue());
        }
        if (!remove_done)
        {
            while (!regular.isEmpty())
            {
                Song temp_song = regular.dequeue();
                if (!remove_done && temp_song.name == songName)
                {
                    remove_done = true;
                }
                else
                {
                    temp_queue.enqueue(temp_song);
                }
            }
            while (!temp_queue.isEmpty())
            {
                regular.enqueue(temp_queue.dequeue());
            }
        }
        if (!remove_done)
        {
            while (!low.isEmpty())
            {
                Song temp_song = low.dequeue();
                if (!remove_done && temp_song.name == songName)
                {
                    remove_done = true;
                }
                else
                {
                    temp_queue.enqueue(temp_song);
                }
            }
            while (!temp_queue.isEmpty())
            {
                low.enqueue(temp_queue.dequeue());
            }
        }
        if (remove_done)
        {
            response << "Removed: " << songName << "." << endl;
        }
        else
        {
            response << "Song wasnt found in the playlist." << endl;
        }
        response.close();
    }
    void show()
    {
        ofstream response("output.txt", ios::app);
        response << "Playlist:" << endl;
        response << "Preference 1:";
        Queue<Song> temp_queue;
        while (!vip.isEmpty())
        {
            Song temp_song = vip.dequeue();
            response << " " << temp_song.name << ", ";
            temp_queue.enqueue(temp_song);
        }
        while (!temp_queue.isEmpty())
        {
            vip.enqueue(temp_queue.dequeue());
        }
        response << endl << "Preference 2:";
        while (!regular.isEmpty())
        {
            Song temp_song = regular.dequeue();
            response << " " << temp_song.name << ", ";
            temp_queue.enqueue(temp_song);
        }
        while (!temp_queue.isEmpty())
        {
            regular.enqueue(temp_queue.dequeue());
        }
        response << endl << "Preference 3:";
        while (!low.isEmpty())
        {
            Song temp_song = low.dequeue();
            response << " " << temp_song.name << ", ";
            temp_queue.enqueue(temp_song);
        }
        while (!temp_queue.isEmpty())
        {
            low.enqueue(temp_queue.dequeue());
        }
        response << endl;
        response.close();
    }
    void history()
    {
        ofstream response("output.txt", ios::app);
        response << "History:" << endl;
        Queue<Song> temp_queue;
        int number = 1;
        while (!playlist_history.isEmpty())
        {
            Song temp_song = playlist_history.dequeue();
            response << number++ << ". " << temp_song.name << "." << endl;
            temp_queue.enqueue(temp_song);
        }
        while (!temp_queue.isEmpty())
        {
            playlist_history.enqueue(temp_queue.dequeue());
        }
        response.close();
    }
};

int main()
{
    ifstream in("input.txt");
    ofstream out("output.txt");
    out.close();
    Playlist playlist;
    string text;
    while (getline(in, text))
    {
        if (text.empty())
        {
            continue;
        }
        istringstream ist(text);
        string action;
        ist >> action;
        if (action == "ADD")
        {
            Song song;
            ist >> song.preference >> song.duration;
            getline(ist, song.name);
            song.name.erase(0, song.name.find_first_not_of(" \t\n\r"));
            playlist.add(song);
        }
        else if (action == "HISTORY")
        {
            playlist.history();
        }
        else if (action == "PLAY")
        {
            playlist.play();
        }
        else if (action == "REPLAY")
        {
            playlist.replay();
        }
        else if (action == "REMOVE")
        {
            string songName;
            getline(ist, songName);
            songName.erase(0, songName.find_first_not_of(" \t\n\r"));
            playlist.remove(songName);
        }
        else if (action == "SKIP")
        {
            int time;
            ist >> time;
            playlist.skip(time);
        }
        else if (action == "SHOW")
        {
            playlist.show();
        }
    }
    in.close();
    return 0;
}
