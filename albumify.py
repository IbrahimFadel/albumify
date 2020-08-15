import spotipy
from spotipy.oauth2 import SpotifyOAuth, SpotifyClientCredentials

client_id = "bc339b67c43d486eadc4c20fc3785390"
client_secret = "ca6c3fbd00c648e7812cb5d81794dc1a"
redirect_uri = "http://localhost:1111/cb"
scope = "user-read-playback-state"

spotify = spotipy.Spotify(auth_manager=SpotifyOAuth(
    scope=scope, cache_path="spotipy_cache", client_id=client_id, client_secret=client_secret, redirect_uri=redirect_uri))


def ms_to_s(ms): return ms / 1000


def get_current_track():
    global spotify

    track = spotify.current_playback()

    return {
        'length': ms_to_s(track['item']['duration_ms']),
        'progress': ms_to_s(track['progress_ms']),
        'img': bytes(track['item']['album']['images'][0]['url'], 'utf-8'),
        'name': bytes(track['item']['name'], 'utf-8')
    }
