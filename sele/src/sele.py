import sys
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
if(len(sys.argv) > 1):
    CHROMEDRIVER_PATH = '/usr/local/bin/chromedriver' #change this according to the place chromdriver is installed
    WINDOW_SIZE = "1920,1080"
    chrome_options = Options()
    chrome_options.add_argument("--headless")
    chrome_options.add_argument("--window-size=%s" % WINDOW_SIZE)
    chrome_options.add_argument('--no-sandbox')
    driver = webdriver.Chrome(executable_path=CHROMEDRIVER_PATH,
                              options=chrome_options
                             )
    driver.get("http://localhost:8081/recordings")
    driver.implicitly_wait(3)#wait 3 seconds for the page to load

    if(sys.argv[1]=='1'):
        driver.find_element_by_xpath('//*[@title="Replay CID-140-recording-2020-03-18_144821-selection.rec."]').click()
        print(".rec found: CID-140-recording-2020-03-18_144821-selection.rec")
    elif(sys.argv[1]=='2'):
        driver.find_element_by_xpath('//*[@title="Replay CID-140-recording-2020-03-18_145043-selection.rec."]').click()
        print(".rec found: CID-140-recording-2020-03-18_145043-selection.rec")
    elif(sys.argv[1]=='3'):
        driver.find_element_by_xpath('//*[@title="Replay CID-140-recording-2020-03-18_145233-selection.rec."]').click()
        print(".rec found: CID-140-recording-2020-03-18_145233-selection.rec")
    elif(sys.argv[1]=='4'):
        driver.find_element_by_xpath('//*[@title="Replay CID-140-recording-2020-03-18_145641-selection.rec."]').click()
        print(".rec found: CID-140-recording-2020-03-18_145641-selection.rec")
    elif(sys.argv[1]=='5'):
        driver.find_element_by_xpath('//*[@title="Replay CID-140-recording-2020-03-18_150001-selection.rec."]').click()
        print(".rec found: CID-140-recording-2020-03-18_150001-selection.rec")
    else:
        print("rec files range from 1 to 5 only!")
    driver.close()
else:
    print("Missing argument!")
sys.exit()
