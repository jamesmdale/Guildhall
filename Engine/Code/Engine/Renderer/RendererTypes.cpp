#include "Engine\Renderer\RendererTypes.hpp"
#include "Engine\Renderer\GlFunctions.hpp"
#include "Engine\Core\EngineCommon.hpp"



int GetGLDrawPrimitive(DrawPrimitiveType type)
{
	//TODO: can add more for other 'BeginMode' open gl types as necessary.
	int glConstant = -1;

	//Quads are depracated
	switch(type)
	{
	case POINT_DRAW_PRIMITIVE:
	{
		glConstant = GL_POINTS;
		break;
	}
	case LINES_DRAW_PRIMITIVE:
	{
		glConstant = GL_LINES;
		break;
	}
	case LINE_LOOP_DRAW_PRIMITIVE:
	{
		glConstant = GL_LINE_LOOP;
		break;
	}
	case TRIANGLES_DRAW_PRIMITIVE:
	{
		glConstant = GL_TRIANGLES;
		break;
	}
	}

	return glConstant;	
}

int GetGLDrawFace(DrawModeFaceType type)
{
	//TODO: can add more for other 'BeginMode' open gl types as necessary.
	int glConstant = -1;

	//Quads are depracated
	switch(type)
	{
	case FRONT_FACE_MODE:
	{
		glConstant = GL_FRONT;
		break;
	}
	case BACK_FACE_MODE:
	{
		glConstant = GL_BACK;
		break;
	}
	case FRONT_AND_BACK_FACE_MODE:
	{
		glConstant = GL_FRONT_AND_BACK;
		break;
	}
	}

	return glConstant;

}

int GetGLDrawMode(DrawModeType type)
{
	//TODO: can add more for other 'BeginMode' open gl types as necessary.
	int glConstant = -1;

	//Quads are depracated
	switch(type)
	{
	case POINT_DRAW_MODE:
	{
		glConstant = GL_POINT;
		break;
	}
	case LINE_DRAW_MODE:
	{
		glConstant = GL_LINE;
		break;
	}
	case FILL_DRAW_MODE:
	{
		glConstant = GL_FILL;
		break;
	}
	}

	return glConstant;

}

int GetGLDepthComparison(DepthComparisonType type)
{
	//TODO: can add more for other 'BeginMode' open gl types as necessary.
	int glConstant = -1;

	//Quads are depracated
	switch(type)
	{
	case NEVER_DEPTH_TYPE:
	{
		glConstant = GL_NEVER;
		break;
	}
	case LESS_DEPTH_TYPE:
	{
		glConstant = GL_LESS;
		break;
	}
	case EQUAL_DEPTH_TYPE:
	{
		glConstant = GL_EQUAL;
		break;
	}
	case LESS_THAN_OR_EQUAL_DEPTH_TYPE:
	{
		glConstant = GL_LEQUAL;
		break;
	}
	case GREATER_DEPTH_TYPE:
	{
		glConstant = GL_GREATER;
		break;
	}
	case NOT_EQUAL_DEPTH_TYPE:
	{
		glConstant = GL_NOTEQUAL;
		break;
	}
	case GREATER_THAN_OR_EQUAL_DEPTH_TYPE:
	{
		glConstant = GL_GEQUAL;
		break;		
	}
	case ALWAYS_DEPTH_TYPE:
	{
		glConstant = GL_ALWAYS;
		break;
	}
	}

	return glConstant;
}

int GetGLRenderDataType(RenderDataType type)
{
	int glConstant = -1;
	switch(type)
	{
	case FLOAT_RENDER_TYPE:
	{
		glConstant = GL_FLOAT;
		break;
	}
	case UNSIGNED_BYTE_RENDER_TYPE:
	{
		glConstant = GL_UNSIGNED_BYTE;
		break;
	}	
	}

	return glConstant;
}

int GetGLBlendFactorType(BlendFactorType type)
{	
	int glConstant = -1;
	switch(type)
	{
	case BLEND_ZERO:
	{
		glConstant = GL_ZERO;
		break;
	}
	case BLEND_ONE:
	{
		glConstant = GL_ONE;
		break;
	}	
	case BLEND_SOURCE_COLOR:
	{
		glConstant = GL_SRC_COLOR;
		break;
	}
	case BLEND_ONE_MINUS_SOURCE_COLOR:
	{
		glConstant = GL_ONE_MINUS_SRC_COLOR;
		break;
	}	
	case BLEND_DESTINATION_COLOR:
	{
		glConstant = GL_DST_COLOR;
		break;
	}	
	case BLEND_ONE_MINUS_DESTINATION_COLOR:
	{
		glConstant = GL_ONE_MINUS_DST_COLOR;
		break;
	}
	case BLEND_SOURCE_ALPHA:
	{
		glConstant = GL_SRC_ALPHA;
		break;
	}	
	case BLEND_ONE_MINUS_SOURCE_ALPHA:
	{
		glConstant = GL_ONE_MINUS_SRC_ALPHA;
		break;
	}			
	case BLEND_DESTINATION_ALPHA:
	{
		glConstant = GL_DST_ALPHA;
		break;
	}	
	case BLEND_ONE_MINUS_DESTINATION_ALPHA:
	{
		glConstant = GL_ONE_MINUS_DST_ALPHA;
		break;
	}	
	case BLEND_CONSTANT_COLOR:
	{
		glConstant = GL_CONSTANT_COLOR;
		break;
	}	
	case BLEND_ONE_MINUS_CONSTANT_COLOR:
	{
		glConstant = GL_ONE_MINUS_CONSTANT_COLOR;
		break;
	}
	case BLEND_CONSTANT_ALPHA:
	{
		glConstant = GL_CONSTANT_ALPHA;
		break;
	}
	case BLEND_ONE_MINUS_CONSTANT_ALPHA:
	{
		glConstant = GL_ONE_MINUS_CONSTANT_ALPHA;
		break;
	}
	}

	return glConstant;
}

int GetGLWindOrderType(WindOrderType type)
{
	int glConstant = -1;
	switch(type)
	{
		case WIND_CLOCKWISE:
		{
			glConstant = GL_CW;
			break;
		}
		case WIND_COUNTER_CLOCKWISE:
		{
			glConstant = GL_CCW;
			break;
		}	
	}

	return glConstant;
}

int GetGLCullModeType(CullModeType type)
{
	int glConstant = -1;
	switch (type)
	{
	case CULL_MODE_BACK:
		glConstant = GL_BACK;
		break;
	case CULL_MODE_FRONT:
		glConstant = GL_FRONT;
		break;
	case CULL_MODE_NONE:
		glConstant = -1;
		break;
	default:
		ASSERT_OR_DIE(false, "UNSUPPORTED GL CULLMODE TYPE");
		break;
	}

	return glConstant;
}

int GetGLBlendOperationType(BlendOperationType type)
{
	int glConstant = -1;
	switch(type)
	{
	case BLEND_OP_ADD:
	{
		glConstant = GL_FUNC_ADD;
		break;
	}
	case BLEND_OP_SUBTRACT:
	{
		glConstant = GL_FUNC_SUBTRACT;
		break;
	}	
	case BLEND_OP_REVERSE_SUBTRACT:
	{
		glConstant = GL_FUNC_REVERSE_SUBTRACT;
		break;
	}	
	case BLEND_OP_MIN:
	{
		glConstant = GL_MIN;
		break;
	}	
	case BLEND_OP_MAX:
	{
		glConstant = GL_MAX;
		break;
	}	
	}

	return glConstant;
}