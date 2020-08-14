#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <iostream>
#include <string>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <curl/curl.h>
#include <unistd.h>

using std::cout;
using std::endl;

static int width = 640;
static int height = 700;
static std::string title = "Spotify Client";

static sf::Texture album_texture;
static sf::Sprite album_sprite;

static sf::RectangleShape duration_bar;
static sf::RectangleShape progress_bar;
static int duration_bar_height = 7;
static int duration_bar_width = width - 100;

static sf::Color duration_bar_bg(55, 55, 55);
static sf::Color progress_bar_bg(190, 190, 190);
static sf::Color bg_color(30, 30, 30);

static sf::Font font;
static sf::Text track_name_text;
static int track_name_text_size = 15;

typedef struct Track
{
	float duration;
	float progress;
	std::string image_url;
	std::string name;
} Track;

Track get_current_track()
{
	setenv("PYTHONPATH", ".", 1);
	Py_Initialize();

	PyObject *pName = PyUnicode_FromString("main");
	PyObject *pModule = PyImport_Import(pName);
	Py_DECREF(pName);

	if (pModule == nullptr)
	{
		PyErr_Print();
		std::exit(1);
	}

	PyObject *pFunc = PyObject_GetAttrString(pModule, "get_current_track");

	if (pFunc && PyCallable_Check(pFunc))
	{
		PyObject *pArgs = PyTuple_New(0);
		PyObject *length_key = PyUnicode_FromString("length");
		PyObject *progress_key = PyUnicode_FromString("progress");
		PyObject *img_key = PyUnicode_FromString("img");
		PyObject *name_key = PyUnicode_FromString("name");
		PyObject *pValue = PyObject_CallObject(pFunc, pArgs);

		if (pValue != NULL)
		{
			PyObject *v = PyObject_GetItem(pValue, length_key);
			Track track;
			track.duration = PyFloat_AsDouble(v);
			v = PyObject_GetItem(pValue, progress_key);
			track.progress = PyFloat_AsDouble(v);
			v = PyObject_GetItem(pValue, img_key);
			char *url = PyBytes_AsString(v);
			track.image_url = PyBytes_AsString(v);
			v = PyObject_GetItem(pValue, name_key);
			track.name = PyBytes_AsString(v);
			Py_DECREF(pValue);

			return track;
		}
		else
		{
			Py_DECREF(pFunc);
			Py_DECREF(pModule);
			PyErr_Print();
			fprintf(stderr, "Call failed\n");
			Track track;
			return track;
		}
	}

	Py_Finalize();

	Track track;
	return track;
}

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string *)userp)->append((char *)contents, size * nmemb);
	return size * nmemb;
}

void update()
{
	Track track = get_current_track();

	CURL *curl;
	CURLcode res;
	std::string read_buffer;

	curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, track.image_url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		std::ofstream file;
		file.open("album.png", std::ios::binary);
		file.write(read_buffer.c_str(), read_buffer.size());
		file.close();
	}
	else
	{
		cout << "Error downloading file: " << track.image_url << endl;
		return;
	}

	if (!album_texture.loadFromFile("album.png"))
	{
		cout << "Error loading album.png" << endl;
		return;
	}

	album_texture.setSmooth(true);

	album_sprite.setTexture(album_texture);
	album_sprite.setPosition(0, 0);

	duration_bar.setFillColor(duration_bar_bg);
	duration_bar.setSize(sf::Vector2f(duration_bar_width, duration_bar_height));

	int duration_bar_y = album_sprite.getLocalBounds().height + 20 - (duration_bar_height / 2);
	int duration_bar_x = 50;
	duration_bar.setPosition(duration_bar_x, duration_bar_y);

	progress_bar.setFillColor(progress_bar_bg);
	progress_bar.setSize(sf::Vector2f((track.progress / track.duration) * duration_bar_width, duration_bar_height));

	int progress_bar_y = album_sprite.getLocalBounds().height + 20 - (duration_bar_height / 2);
	int progress_bar_x = 50;
	progress_bar.setPosition(progress_bar_x, progress_bar_y);

	if (!font.loadFromFile("assets/Lato/Lato-Regular.ttf"))
	{
		cout << "Error loading font" << endl;
		return;
	}

	track_name_text.setFont(font);
	track_name_text.setString(track.name);
	track_name_text.setCharacterSize(track_name_text_size);
	track_name_text.setFillColor(sf::Color::White);

	int track_name_text_x = (width / 2) - track_name_text.getLocalBounds().width / 2;
	int track_name_text_y = album_sprite.getLocalBounds().height + 30;
	track_name_text.setPosition(track_name_text_x, track_name_text_y);
}

int main()
{
	sf::RenderWindow *window = new sf::RenderWindow(sf::VideoMode(width, height), title);
	window->setFramerateLimit(60);

	while (window->isOpen())
	{
		sf::Event event;

		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window->close();
		}

		usleep(500000);

		update();

		window->clear(bg_color);

		window->draw(album_sprite);

		window->draw(duration_bar);
		window->draw(progress_bar);

		window->draw(track_name_text);

		window->display();
	}

	return 0;
}