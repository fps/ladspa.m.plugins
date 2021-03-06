#include <ladspa.h>
#include <cmath>

static const int LADSPAM_NUMBER_OF_PLUGINS = 3;
static LADSPA_Descriptor ladspam_descriptors[LADSPAM_NUMBER_OF_PLUGINS];

static const int LADSPAM_NUMBER_OF_PORTS = 4;

static const char* ladspam_port_names[] = 
{
	"Frequency",
	"Phase",
	"Trigger",
	"Out"
};

static const int FREQUENCY = 0;
static const int PHASE = 1;
static const int TRIGGER = 2;
static const int OUT = 3;

static LADSPA_PortDescriptor ladspam_port_descriptors[] = 
{
	LADSPA_PORT_AUDIO | LADSPA_PORT_INPUT,
	LADSPA_PORT_AUDIO | LADSPA_PORT_INPUT,
	LADSPA_PORT_AUDIO | LADSPA_PORT_INPUT,
	LADSPA_PORT_AUDIO | LADSPA_PORT_OUTPUT
};

static LADSPA_PortRangeHint ladspam_port_range_hints[] = 
{
	{ LADSPA_HINT_SAMPLE_RATE | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_440, 0.0, 0.5 },
	{ LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE, (float)(-2.0*M_PI), (float)(2.0*M_PI) },
	{ LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MINIMUM, 0.0, 1.0 },
	{ 0, 0.0, 0.0 }
};

struct ladspam_plugin
{
	float m_phase;

	float m_last_trigger;

	float *m_ports[LADSPAM_NUMBER_OF_PORTS];

	unsigned long m_sample_rate;

	ladspam_plugin(unsigned long sample_rate) :
		m_phase(0),
		m_last_trigger(0),
		m_sample_rate(sample_rate)
	{

	}
};

static LADSPA_Handle ladspam_instantiate(const LADSPA_Descriptor *descriptor, unsigned long sample_rate)
{
	return new ladspam_plugin(sample_rate);	
}

static void ladspam_cleanup(LADSPA_Handle instance)
{
	delete (ladspam_plugin*)instance;
}

static void ladspam_connect_port(LADSPA_Handle instance, unsigned long port, LADSPA_Data * data_location)
{
	((ladspam_plugin*)instance)->m_ports[port] = data_location;
}

static void ladspam_run_sine(LADSPA_Handle instance, unsigned long sample_count)
{
	ladspam_plugin &i = *(ladspam_plugin*)instance;
	for (unsigned long index = 0; index < sample_count; ++index)
	{
		float trigger = i.m_ports[TRIGGER][index];
		if (trigger != 0 && i.m_last_trigger == 0) 
		{
			i.m_phase = 0;
		}
		i.m_last_trigger = trigger;

		i.m_ports[OUT][index] = sinf(i.m_phase + i.m_ports[PHASE][index]);

		i.m_phase += i.m_ports[FREQUENCY][index] * 2.0f * (float)M_PI / (float)i.m_sample_rate;
		i.m_phase = fmodf(i.m_phase, 2.0f * (float)M_PI);
	}
}

static void ladspam_run_sawtooth(LADSPA_Handle instance, unsigned long sample_count)
{
	ladspam_plugin &i = *(ladspam_plugin*)instance;
	for (unsigned long index = 0; index < sample_count; ++index)
	{
		float trigger = i.m_ports[TRIGGER][index];
		if (trigger != 0 && i.m_last_trigger == 0) 
		{
			i.m_phase = 0;
		}
		i.m_last_trigger = trigger;

		i.m_phase += i.m_ports[FREQUENCY][index] * 2.0f * (float)M_PI / (float)i.m_sample_rate;
		i.m_phase = fmodf(i.m_phase, 2.0f * (float)M_PI);
		i.m_ports[OUT][index] = (i.m_phase + i.m_ports[PHASE][index] - (float)M_PI)/(float)M_PI;
	}
}

static void ladspam_run_square(LADSPA_Handle instance, unsigned long sample_count)
{
	ladspam_plugin &i = *(ladspam_plugin*)instance;
	for (unsigned long index = 0; index < sample_count; ++index)
	{
		float trigger = i.m_ports[TRIGGER][index];
		if (trigger != 0 && i.m_last_trigger == 0) 
		{
			i.m_phase = 0;
		}
		i.m_last_trigger = trigger;

		i.m_phase += i.m_ports[FREQUENCY][index] * 2.0f * (float)M_PI / (float)i.m_sample_rate;
		i.m_phase = fmodf(i.m_phase, 2.0f * (float)M_PI);
		i.m_ports[OUT][index] = (i.m_phase + i.m_ports[PHASE][index]) > (float)M_PI ? -1.0f : 1.0f;
	}
}

const LADSPA_Descriptor* ladspa_descriptor(unsigned long Index)
{
	if (Index >= LADSPAM_NUMBER_OF_PLUGINS) return 0;

	// static LADSPA_Descriptor *d = new LADSPA_Descriptor;
	LADSPA_Descriptor *d = &ladspam_descriptors[Index];

	d->UniqueID = 0;

	d->Properties = LADSPA_PROPERTY_REALTIME | LADSPA_PROPERTY_HARD_RT_CAPABLE;
	d->Maker = "Florian Paul Schmidt";
	d->Copyright = "GPL v2 or later";
	d->PortCount = LADSPAM_NUMBER_OF_PORTS;
	
	d->PortDescriptors = ladspam_port_descriptors;
	d->PortRangeHints = ladspam_port_range_hints;

	d->instantiate = ladspam_instantiate;
	d->cleanup = ladspam_cleanup;
	d->connect_port = ladspam_connect_port;

	d->run_adding = 0;
	d->set_run_adding_gain = 0;
	d->activate = 0;
	d->deactivate = 0;
	
	d->PortNames = ladspam_port_names;

	d->ImplementationData = (void *)Index;

	switch(Index)
	{
		case 0:
			d->Label = "ladspa.m.sine.osc";
			d->Name = "ladspam sine oscillator";
			d->run = ladspam_run_sine;
			break;
		case 1:
			d->Label = "ladspa.m.saw.osc";
			d->Name = "ladspam sawtooth oscillator";
			d->run = ladspam_run_sawtooth;
			break;
		case 2:
			d->Label = "ladspa.m.square.osc";
			d->Name = "ladspam square oscillator";
			d->run = ladspam_run_square;
			break;
	}

	return d;
}

#ifdef LADSPAM_MAIN_CC

#include <iostream>
#include <vector>

int main()
{
	const unsigned long sample_rate = 1000;
	const float frequency = 1.0;
	const unsigned long buffer_size = 2000;
	const LADSPA_Descriptor *d = ladspa_descriptor(0);
	LADSPA_Handle h = d->instantiate(d, sample_rate);
	std::vector<std::vector<float>> buffers;
	for (size_t index = 0; index < LADSPAM_NUMBER_OF_PORTS; ++index)
	{
		buffers.push_back(std::vector<float>(buffer_size,0));
		d->connect_port(h, index, &(buffers[index][0]));
	}
	for (size_t index = 0; index < buffer_size; ++index)
	{
		buffers[0][index] = frequency;
		buffers[1][index] = 0;
		buffers[2][index] = 0;
	}
	d->run(h, buffer_size);
	for (size_t index = 0; index < buffer_size; ++index)
	{
		std::cout << buffers[3][index] << std::endl;
	}
	d->cleanup(h);
}

#endif

