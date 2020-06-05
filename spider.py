import requests
from fake_useragent import UserAgent
from bs4 import BeautifulSoup
from selenium import webdriver
from movie import movie

url_base = 'https://movie.douban.com/top250?start='
options = webdriver.ChromeOptions()
# options.add_argument('-headless')
driver = webdriver.Chrome(options = options)

def get_movie_url():
    headers = { 'User-Agent' : str(UserAgent().random)}
    i = 0
    while (i < 226):
        url_to_get = url_base + str(i) + '&filter='
        response = requests.get(url = url_to_get, headers=headers, timeout = 10)
        soup = BeautifulSoup(response.text, 'lxml')
        urls = soup.select('div.info > div.hd > a')
        for elems in urls:
            get_movie_elems(elems['href'])
        i += 25

    driver.close()


def get_movie_elems(movie_url):
    driver.get(movie_url)
    soup = BeautifulSoup(driver.page_source, 'lxml')
    movie_name = soup.select_one('span[property="v:itemreviewed"]')
    movie_year = soup.select_one('span[class="year"]')
    movie_infomation = soup.select_one('div[id="info"]')
    movie_score = soup.select_one('strong[property="v:average"]')
    if movie_name:
        with open('movie.txt', 'a+', encoding = 'utf-8') as f:
            f.write(movie_name.text + '\n')
            f.write(movie_year.text)
            f.write(movie_infomation.text)
            f.write(movie_score.text)
            f.write('\n\n')
            f.close()
    else:
        get_movie_elems(movie_url)
    input()