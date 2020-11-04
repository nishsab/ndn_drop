#!/usr/bin/python

import sys
import ndn.utils
from ndn.app import NDNApp
from ndn.types import InterestNack, InterestTimeout, InterestCanceled, ValidationFailure
from ndn.encoding import Name, Component, InterestParam

app = NDNApp()

async def main():
    try:
        interest_name = sys.argv[1]
        print("Requesting " + interest_name)
        timestamp = ndn.utils.timestamp()
        name = Name.from_str(interest_name)
        print(f'Sending Interest {Name.to_str(name)}, {InterestParam(must_be_fresh=True, lifetime=6000)}')
        data_name, meta_info, content = await app.express_interest(
            name, must_be_fresh=False, can_be_prefix=True, lifetime=6000)

        print(f'Received Data Name: {Name.to_str(data_name)}')
        print(meta_info)
        print(bytes(content) if content else None)
    except InterestNack as e:
        print(f'Nacked with reason={e.reason}')
    except InterestTimeout:
        print(f'Timeout')
    except InterestCanceled:
        print(f'Canceled')
    except ValidationFailure:
        print(f'Data failed to validate')
    finally:
        app.shutdown()


if __name__ == '__main__':
    app.run_forever(after_start=main())
