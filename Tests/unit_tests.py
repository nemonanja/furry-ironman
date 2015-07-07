
import requests
import httpretty
from sure import expect
import unittest
import flask

#r = requests.get('http://teemupa.dy.fi:8080/domain/endpoints/monitor-22351/sen/weight?sync=true', auth=('admin', 'secret'))
#print(r.status_code)

user = 'admin'
password = 'secret'

class ResourcesAPITestCase(unittest.TestCase):

    def setUp(self)
        self.client = requests.app.test_client()




class WeigthTestCase():

    @classmethod
    def setUpClass(cls):
        print 'Testing WeightTestCase'

    @httpretty.activate
    def test_yipit_api_returning_deals(self):
        url = 'http://teemupa.dy.fi:8080/domain/endpoints/monitor-22351/sen/weight?sync=true'
        httpretty.register_uri(httpretty.GET, url,
                           status = 200)

        response = requests.get('http://teemupa.dy.fi:8080/domain/endpoints/monitor-22351/sen/weight?sync=true', auth=(user, password))

        expect(response.status_code).to.equal(200)


    def test_url(self):
        '''
        Checks that the URL points to the right resource
        '''

        url = 'http://teemupa.dy.fi:8080/domain/endpoints/monitor-22351/sen/weight?sync=true'

        print '('+self.test_url.__name__+')', self.test_url.__doc__,
        with url:
            response = requests.get('http://teemupa.dy.fi:8080/domain/endpoints/monitor-22351/sen/weight?sync=true', auth=(user, password))
            resource = response.url
            self.assertEquals(url, resource)



if __name__ == '__main__':
    print 'Start running tests'
    unittest.main()