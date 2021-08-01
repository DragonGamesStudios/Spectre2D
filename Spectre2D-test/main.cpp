#define SPECTRE_DEBUG

#include <Spectre2D/System.hpp>
#include <Spectre2D/Graphics.hpp>
#include <Spectre2D/Math.hpp>

#include <GL/glew.h>

#include <iostream>

void GetFirstNMessages(GLuint numMsgs)
{
	GLint maxMsgLen = 0;
	glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH, &maxMsgLen);

	std::vector<GLchar> msgData(numMsgs * maxMsgLen);
	std::vector<GLenum> sources(numMsgs);
	std::vector<GLenum> types(numMsgs);
	std::vector<GLenum> severities(numMsgs);
	std::vector<GLuint> ids(numMsgs);
	std::vector<GLsizei> lengths(numMsgs);

	GLuint numFound = glGetDebugMessageLog(numMsgs, msgData.size(), &sources[0], &types[0], &ids[0], &severities[0], &lengths[0], &msgData[0]);

	sources.resize(numFound);
	types.resize(numFound);
	severities.resize(numFound);
	ids.resize(numFound);
	lengths.resize(numFound);

	std::vector<std::string> messages;
	messages.reserve(numFound);

	std::vector<GLchar>::iterator currPos = msgData.begin();
	for (size_t msg = 0; msg < lengths.size(); ++msg)
	{
		messages.push_back(std::string(currPos, currPos + lengths[msg] - 1));
		currPos = currPos + lengths[msg];
	}

	for (size_t i = 0; i < numFound; i++)
		std::cout << "::OPENGL_DEBUG_LOG::\n"
		<< "Source: " << sources[i] << '\n'
		<< "Type: " << types[i] << '\n'
		<< "Severity: " << severities[i] << '\n'
		<< "ID: " << ids[i]
		<< "Message: " << messages[i] << "\n::OPENGL_DEBUG_LOG::\n\n";
}

int main()
{
	try
	{
		sp::init();

		sp::RenderWindow wnd;

		wnd.setVSyncEnabled(true);
		wnd.setResizable(true);
		wnd.setFlag(sp::WindowFlagB::DebugContext, true);

		wnd.create({ 800, 600 }, "Spectre2D test");

		sp::Shape shape;
		
		shape.createMesh();
		shape.createVertexData();

		shape.createLine({ {100.f, 100.f}, {100.f, 200.f}, {200.f, 100.f} });

		wnd.setLineWidth(10.0f);
 
		sp::Shader* sh = wnd.getActiveShader();

		sh->setUniform("sp_textureAssociated", false);

		while (!wnd.shouldClose())
		{
			sp::pollEvents();

			double time = sp::getTime();

			wnd.setBackgroundColor(glm::vec4( (float)sin(time) / 2 + 0.5f, (float)cos(time) / 2 + 0.5f, (float)abs(tan(time)), 1.0f ));
			shape.setTint(glm::vec4( (float)cos(time) / 2 + 0.5f, (float)abs(tan(time)), (float)sin(time) / 2 + 0.5f, 1.0f));

			wnd.clear();

			wnd.draw(&shape);

			wnd.flip();
		}

		GetFirstNMessages(100);
		
		sp::finish();
	}
	catch (const sp::Error& err)
	{
		std::cout << err.code << ": " << err.description << std::endl;
	}
}