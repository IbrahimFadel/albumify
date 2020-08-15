import spotipy
from spotipy.oauth2 import SpotifyOAuth

scope = "user-read-playback-state"

spotify = spotipy.Spotify(auth_manager=SpotifyOAuth(
    scope=scope, cache_path="spotipy_cache"))


def ms_to_s(ms): return ms / 1000


def get_current_track():
    global spotify

    track = spotify.current_playback()
    return {
        'length': ms_to_s(track['item']['duration_ms']),
        'progress': ms_to_s(track['progress_ms']),
        'img': bytes(track['item']['album']['images'][0]['url'], 'utf-8'),
        'name': bytes(track['item']['name'], 'utf-8'),
        'artist': bytes(track['item']['artists'][0]['name'], 'utf-8'),
        'album': bytes(track['item']['album']['name'], 'utf-8'),
        'playing': track['is_playing']
    }
