import time
from py2neo import Graph,Node,Relationship, NodeMatcher

graph = Graph('http://localhost:7474', auth = ("neo4j", "looser.."))

class movie:
    movie_name = []
    movie_year = ''
    movie_director = []
    movie_screenwriter = []
    movie_actor = []
    movie_type = []
    movie_country = []
    movie_language = []
    movie_time = []
    movie_duration = []
    movie_other_name = []
    movie_score = ''

    def show_movie(self):
        with open('movie_data.txt', 'a+', encoding = 'utf-8') as f:
            f.write('电影名字: ')
            for i in self.movie_name:
                f.write(i + ' ')
            f.write('\n')
            
            f.write('电影年份: ')
            f.write(self.movie_year)
            f.write('\n')

            f.write('电影导演: ')
            for i in self.movie_director:
                f.write(i + ' ')
            f.write('\n')

            f.write('电影编剧: ')
            for i in self.movie_screenwriter:
                f.write(i + ' ')
            f.write('\n')

            f.write('电影主演: ')
            for i in self.movie_actor:
                f.write(i + ' ')
            f.write('\n')

            f.write('电影类型: ')
            for i in self.movie_type:
                f.write(i + ' ')
            f.write('\n')

            f.write('电影国家: ')
            for i in self.movie_country:
                f.write(i + ' ')
            f.write('\n')

            f.write('电影语言: ')
            for i in self.movie_language:
                f.write(i + ' ')
            f.write('\n')

            f.write('电影日期: ')
            for i in self.movie_time:
                f.write(i + ' ')
            f.write('\n')

            f.write('电影时长: ')
            for i in self.movie_duration:
                f.write(i + ' ')
            f.write('\n')

            f.write('电影别名: ')
            for i in self.movie_other_name:
                f.write(i + ' ')
            f.write('\n')

            f.write('电影评分: ')
            f.write(self.movie_score)
            f.write('\n')

    def get_name(self, name_s):
        length = len(name_s)
        i = 0
        while (i < length) and (name_s[i] != ' '):
            i += 1
        if i == length:
            name1 = name_s[0 : length - 1]
            self.movie_name.append(name1)
        else:
            name1 = name_s[0 : i]
            name2 = name_s[i + 1 : length - 1]
            self.movie_name.append(name1)
            self.movie_other_name.append(name2)

    def get_year(self, year_s):
        length = len(year_s)
        self.movie_year = year_s[1 : length - 2]

    def get_director(self, director_s):
        i = 4
        j = 0
        length = len(director_s)
        while (j < length):
            if director_s[j] == '/':
                self.movie_director.append(director_s[i : j - 1])
                i = j + 2
                j += 1
            else:
                j += 1
        self.movie_director.append(director_s[i : j - 1])

    def get_screenwriter(self, screenwriter_s):
        i = 4
        j = 0
        length = len(screenwriter_s)
        while (j < length):
            if screenwriter_s[j] == '/':
                self.movie_screenwriter.append(screenwriter_s[i : j - 1])
                i = j + 2
                j += 1
            else:
                j += 1
        self.movie_screenwriter.append(screenwriter_s[i : j - 1])

    def get_actor(self, actor_s):
        i = 4
        j = 0
        actor_s = actor_s.replace('更多...', '')
        length = len(actor_s)
        while (j < length):
            if actor_s[j] == '/':
                self.movie_actor.append(actor_s[i : j - 1])
                i = j + 2
                j += 1
            else:
                j += 1
        self.movie_actor.append(actor_s[i : j - 1])

    def get_type(self, type_s):
        i = 4
        j = 0
        length = len(type_s)
        while (j < length):
            if type_s[j] == '/':
                self.movie_type.append(type_s[i : j - 1])
                i = j + 2
                j += 1
            else:
                j += 1
        self.movie_type.append(type_s[i : j - 1])

    def get_country(self, country_s):
        i = 9
        j = 9
        length = len(country_s)
        while (j < length):
            if country_s[j] == '/':
                self.movie_country.append(country_s[i : j - 1])
                i = j + 2
                j += 1
            else:
                j += 1
        self.movie_country.append(country_s[i : j - 1])

    def get_language(self, language_s):
        i = 4
        j = 4
        length = len(language_s)
        while (j < length):
            if language_s[j] == '/':
                self.movie_language.append(language_s[i : j - 1])
                i = j + 2
                j += 1
            else:
                j += 1
        self.movie_language.append(language_s[i : j - 1])

    def get_time(self, time_s):
        i = 6
        j = 6
        length = len(time_s)
        while (j < length):
            if time_s[j] == '/':
                self.movie_time.append(time_s[i : j - 1])
                i = j + 2
                j += 1
            else:
                j += 1
        self.movie_time.append(time_s[i : j - 1])

    def get_duration(self, duration_s):
        i = 4
        j = 4
        length = len(duration_s)
        while (j < length):
            if duration_s[j] == '/':
                self.movie_duration.append(duration_s[i : j - 1])
                i = j + 2
                j += 1
            else:
                j += 1
        self.movie_duration.append(duration_s[i : j - 1])

    def get_other_name(self, other_name_s):
        if other_name_s == '':
            return
        i = 4
        j = 4
        length = len(other_name_s)
        while (j < length):
            if other_name_s[j] == '/':
                self.movie_other_name.append(other_name_s[i : j - 1])
                i = j + 2
                j += 1
            else:
                j += 1
        self.movie_other_name.append(other_name_s[i : j - 1])

    def get_score(self, score_s):
        self.movie_score = score_s

    def clear(self):
        self.movie_name = []
        self.movie_year = ''
        self.movie_director = []
        self.movie_screenwriter = []
        self.movie_actor = []
        self.movie_type = []
        self.movie_country = []
        self.movie_language = []
        self.movie_time = []
        self.movie_duration = []
        self.movie_other_name = []

    def get_detial(self, movie_s):
        if movie_s[0 : 2] == '导演':
            self.get_director(movie_s)
        elif movie_s[0 : 2] == '编剧':
            self.get_screenwriter(movie_s)
        elif movie_s[0 : 2] == '主演':
            self.get_actor(movie_s)
        elif movie_s[0 : 2] == '类型':
            self.get_type(movie_s)
        elif movie_s[0 : 2] == '制片':
            self.get_country(movie_s)
        elif movie_s[0 : 2] == '语言':
            self.get_language(movie_s)
        elif movie_s[0 : 2] == '上映':
            self.get_time(movie_s)
        elif movie_s[0 : 2] == '片长':
            self.get_duration(movie_s)
        elif movie_s[0 : 2] == '又名':
            self.get_other_name(movie_s)

    def write_to_neo4j(self):
        matcher = NodeMatcher(graph)
        
        year_node = Node()
        matchResult = list(matcher.match('year', name = self.movie_year))
        if len(matchResult):
            year_node = matchResult[0]
        else:
            year_node = Node('year', name = self.movie_year)
        graph.create(year_node)

        movie_node = Node()
        matchResult = list(matcher.match('movie', name = self.movie_name[0]))
        if len(matchResult):
            movie_node = matchResult[0]
        else:
            movie_node = Node('movie', name = self.movie_name[0])
        graph.create(movie_node)
        
        relationShip = Relationship(year_node, '拍摄了', movie_node)
        graph.create(relationShip)
        relationShip = Relationship(movie_node, '拍摄于', year_node)
        graph.create(relationShip)

        director_node = Node()
        for director_name in self.movie_director:
            matchResult = list(matcher.match('director', name = director_name))
            if len(matchResult):
                director_node = matchResult[0]
            else:
                director_node = Node('director', name = director_name)
            graph.create(director_node)
            
            relationShip = Relationship(movie_node, '被导演', director_node)
            graph.create(relationShip)
            relationShip = Relationship(director_node, '导演了', movie_node)
            graph.create(relationShip)
            
        
        screenwriter_node = Node()
        for screenwriter_name in self.movie_screenwriter:
            matchResult = list(matcher.match('screenWriter', name = screenwriter_name))
            if len(matchResult):
                screenwriter_node = matchResult[0]
            else:
                screenwriter_node = Node('screenwriter', name = screenwriter_name)
            graph.create(screenwriter_node)

            relationShip = Relationship(movie_node, '被编剧', screenwriter_node)
            graph.create(relationShip)
            relationShip = Relationship(screenwriter_node, '编剧了', movie_node)
            graph.create(relationShip)

        actor_node = Node()
        for actor_name in self.movie_actor:
            matchResult = list(matcher.match('actor', name = actor_name))
            if len(matchResult):
                actor_node = matchResult[0]
            else:
                actor_node = Node('actor', name = actor_name)
            graph.create(actor_node)

            relationShip = Relationship(movie_node, '被主演', actor_node)
            graph.create(relationShip)
            relationShip = Relationship(actor_node, '主演了', movie_node)
            graph.create(relationShip)
        
        type_node = Node()
        for type_name in self.movie_type:
            matchResult = list(matcher.match('type', name = type_name))
            if len(matchResult):
                type_node = matchResult[0]
            else:
                type_node = Node('type', name = type_name)
            graph.create(type_node)

            relationShip = Relationship(movie_node, '属于', type_node)
            graph.create(relationShip)
            relationShip = Relationship(type_node, '包含', movie_node)
            graph.create(relationShip)

        country_node = Node()
        for country_name in self.movie_country:
            matchResult = list(matcher.match('country', name = country_name))
            if len(matchResult):
                country_node = matchResult[0]
            else:
                country_node = Node('country', name = country_name)
            graph.create(country_node)

            relationShip = Relationship(movie_node, '拍摄于', country_node)
            graph.create(relationShip)
            relationShip = Relationship(country_node, '拍摄了', movie_node)
            graph.create(relationShip)


        
        movie_node['language'] = self.movie_language
        movie_node['time'] = self.movie_time
        movie_node['duration'] = self.movie_duration
        movie_node['score'] = self.movie_score
        movie_node['other_name'] = self.movie_other_name
        


def write_to_database():
    graph.delete_all()
    mve = movie()
    with open('movie.txt', 'r', encoding = 'utf-8') as f:
        i = 1
        s = f.readline()
        while s:
            mve.get_name(s)
            print('writing tne num ' + str(i) + ' movie : ' + mve.movie_name[0])
            i += 1
            s = f.readline()
            mve.get_year(s)
            s2 = f.readline()
            while s2 != '\n':
                s = s2
                mve.get_detial(s)
                s2 = f.readline()
            mve.get_score(s)
            mve.show_movie()
            mve.write_to_neo4j()
            mve.clear()
            s = f.readline()

