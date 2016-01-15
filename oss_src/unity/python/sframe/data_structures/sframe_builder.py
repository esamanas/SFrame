"""
class SArrayBuilder(object):
    #TODO: What would a parallel write look like from this interface?
    def __init__(self, num_segments=1):
        from .. import extensions
        self._builder = extensions.sarray_builder(num_segments)

    #TODO: Take generator/generator expressions?
    def append(self, data, segment=0):
        # ??? What data will this accept?
        self._builder.append(data, segment)

    def close(self):
        self._builder.close()
"""


