from typing import Any, Callable
import socket
from dataclasses import dataclass
from enum import Enum
import json
from pprint import pprint
from time import sleep

SERVER_PORT: int = 49153


def try_again(*, try_for: int) -> Callable:  # what gets called, params to decorator
    """
    runs a function that throws an exception until the function don't throw exception
    :param try_for: max number of trys (stop after even if still throws)
    :return: DECORATOR
    """
    def decorator(f: Callable) -> Callable:  # pass the function of the decprator
        def wrapper(*args, **kwargs) -> None:  # returns the decorated func, deals with args
            for i in range(1, try_for):
                try:
                    return f(*args, **kwargs)
                except Exception:
                    print(f"Falid to do '{f.__name__}', try again... ({i}/{try_for})")
                    sleep(2.5)

            # if all retries failed, raise the last exception
            raise Exception(f"Function failed after {try_for} attempts")

        return wrapper
    return decorator


class ClientCodes(Enum):
    """
    # = one that have a  method
    """
    GENERAL_ERROR = 50
    SIGN_UP_REQUEST = 51  #
    LOG_IN_REQUEST = 52  #
    LOG_OUT_REQUEST = 53  #

    PLAYERS_IN_ROOM_REQUEST = 54  #
    JOIN_ROOM_REQUEST = 55  #
    CREATE_ROOM_REQUEST = 56  #
    GET_HIGH_SCORE_REQUEST = 57
    GET_PRSONAL_STAT_REQUEST = 58  #
    GET_ROOMS_REQUEST = 59  #

    CLOSE_ROOM_REQUEST = 60  #
    START_GAME_REQUEST = 61  #
    GET_ROOM_STATE_REQUEST = 62  #
    LEAVE_ROOM_REQUEST = 63  #

    LEAVE_GAME_REQUEST = 67  #
    GET_QUESTION_REQUEST = 68  #
    SUBMIT_ANS_REQUEST = 69  #
    GET_GAME_RESULTS_REQUEST = 70  #


@dataclass
class Client:
    port: int
    username: str
    password: str
    email: str
    host: str = 'localhost'
    sock: socket or None = None

    def __post_init__(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.bind((self.host, self.port))

        self.sock.connect((self.host, SERVER_PORT))

    def __send(self, code: int, js: json, *, val_to_return: str or None = None) -> Any or None:
        my_bytes = bytearray()
        my_bytes.append(code)
        length_bytes = len(js).to_bytes(4, byteorder='big')
        my_bytes.extend(length_bytes)

        print(f'Client {self.port} sending msg code {code}')
        self.sock.sendall(my_bytes + json.dumps(js).encode('utf-8'))

        response_raw = self.sock.recv(10_000).decode()
        print(f'Server msg to client {self.port} - ')
        try:
            #  print(f"first 5 bytes: {[b for b in response_raw[:9].encode('latin1')]}", end='\n\n')
            #  print(f'sum of len - {sum([b for b in response_raw[1:9].encode('latin1')])}')
            #  print(f'len of actual json - {len(response_raw[9:])}')
            pprint(json.loads(response_raw[9:]), width=110, compact=True, sort_dicts=False)
            response_json = json.loads(response_raw[9:])
            if val_to_return and val_to_return in response_json:
                return response_json[val_to_return]
        except json.JSONDecodeError as e:
            print(e.msg)
            print(response_raw[5:])

        return None

    def close_sock_unexpectedly(self) -> None:
        """
        close the pipe of the sock to demenstarate a crath in the client
        """
        self.sock.shutdown(socket.SHUT_RDWR)

    def get_urilated_ans(self) -> None:
        """
        gets an answer from the client without a connection to a request
        """
        response_raw = self.sock.recv(10_000).decode()
        print(f'Server msg to client {self.port} - ')
        try:
            pprint(json.loads(response_raw[5:]))
        except json.JSONDecodeError as e:
            print(e.msg)
            print(response_raw[5:])

    def login(self) -> None:
        """
        send a login request to the server
        print answer
        """
        js: json = {
            "username": self.username,
            "password": self.password
        }

        self.__send(ClientCodes.LOG_IN_REQUEST.value, js)

    def logout(self) -> None:
        """
        sends a logout request to the server
        print answer
        """
        js: json = {
            "username": self.username
        }

        self.__send(ClientCodes.LOG_OUT_REQUEST.value, js)

    def signup(self) -> None:
        """
        sends a sighup request to the server
        print answer
        """
        js: json = {
            "username": self.username,
            "password": self.password,
            "email": self.email
        }

        self.__send(ClientCodes.SIGN_UP_REQUEST.value, js)

    def create_room(self, room_name: str, *,
                    max_players: int = 30, num_of_questions: int = 22, time_per_question: int = 45,
                    deep_check: bool = False) -> int:
        """
        sends create room request to the server
        print answer
        :param deep_check: with deep check = True. The function will not try to return the room ID.
        :param room_name: the room name to create on the server
        :param max_players: max players that can be in the room
        :param num_of_questions: number of questions that will be on the room
        :param time_per_question: time to have for each question
        :return: the room ID that created
        """
        js: json = {
            "name": room_name,
            "username": self.username,
            "maxPlayers": max_players,
            "numOfQuestions": num_of_questions,
            "timePerQuestion": time_per_question
        }

        if not deep_check:
            return int(self.__send(ClientCodes.CREATE_ROOM_REQUEST.value, js, val_to_return="id"))
        else:
            self.__send(ClientCodes.CREATE_ROOM_REQUEST.value, js)
            return 0

    def join_room(self, room_id: int) -> None:
        """
        sends a join room request to the server
        print answer
        :param room_id: the room ID to join to
        """
        js: json = {
            "id": room_id,
            "username": self.username
        }

        self.__send(ClientCodes.JOIN_ROOM_REQUEST.value, js)

    def get_players_in_room(self, room_id: int) -> None:
        """
        ask from the server the players in a room
        print answer
        :param room_id: the room ID to get its players
        """
        js: json = {
            "id": room_id
        }

        self.__send(ClientCodes.PLAYERS_IN_ROOM_REQUEST.value, js)

    def get_personal_stats(self) -> None:
        """
        ask the server to personal statistics
        print answer
        """
        js: json = {
            "username": self.username
        }

        self.__send(ClientCodes.GET_PRSONAL_STAT_REQUEST.value, js)

    def get_rooms(self) -> None:
        """
        ask the server for a list of all rooms
        """
        self.__send(ClientCodes.GET_ROOMS_REQUEST.value, {})

    def get_high_score(self) -> None:
        """
        ask the server for the top 5 players
        print answer
        """
        self.__send(ClientCodes.GET_HIGH_SCORE_REQUEST.value, {})

    def close_room(self) -> None:
        """
        ask the server to close the room (only as admin)
        print answer
        """
        self.__send(ClientCodes.CLOSE_ROOM_REQUEST.value, {})

    def start_game(self) -> None:
        """
        ask the server to start the game in the room (only as admin)
        print answer
        """
        self.__send(ClientCodes.START_GAME_REQUEST.value, {})

    def get_room_state(self) -> None:
        """
        ask the server for the room state (room the user in)
        print answer
        """
        self.__send(ClientCodes.GET_ROOM_STATE_REQUEST.value, {})

    def leave_room(self) -> None:
        """
        leave the room currently in
        print answer
        """
        self.__send(ClientCodes.LEAVE_ROOM_REQUEST.value, {})

    def leave_game(self) -> None:
        """
        leave the game currently in
        print answer
        """
        self.__send(ClientCodes.LEAVE_GAME_REQUEST.value, {})

    def get_question(self) -> None:
        """
        ask from the server the next question in the game currently in
        print answer
        """
        self.__send(ClientCodes.GET_QUESTION_REQUEST.value, {})

    def submit_answer(self, ans_id: int, wait_for=0) -> None:
        """
        submit an answer when in a game
        :param wait_for: the time to wait until submit (symulate real world waitting)
        :param ans_id: the answer chose by the user to be the corrent one
        """
        js: json = {
            'ansID': ans_id
        }

        sleep(wait_for)
        self.__send(ClientCodes.SUBMIT_ANS_REQUEST.value, js)

    def get_game_results(self) -> None:
        """
        ask of game results (after game ended)
        print answer
        """
        self.__send(ClientCodes.GET_GAME_RESULTS_REQUEST.value, {})

    def question_answer_loop(self, num_of_questions: int, wait_for: int = 0) -> None:
        """
        do a correct question-answer loop the number of times given
        :param num_of_questions: the number of times to do the loop
        :param wait_for: the time to wait before sending the submition (answer)
        """
        for i in range(num_of_questions):
            self.get_question()
            self.submit_answer(1, wait_for=wait_for)
